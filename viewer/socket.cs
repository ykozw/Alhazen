using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace alhzen_viewer
{
    public class SocketStream
    {
        public byte[] data_ = null;
        public int pos_ = 0;

        public SocketStream(byte[] data)
        {
            data_ = data;
        }
        //
        public byte popUint8()
        {
            byte b = data_[pos_++];
            return b;
        }
        public UInt16 popUint16()
        {
            return
                (UInt16)(
                (data_[pos_++] << 8) |
                (data_[pos_++] << 0));
        }
        public UInt32 popUint32()
        {
            return
                (UInt32)(
                (data_[pos_++] << 24) |
                (data_[pos_++] << 16) |
                (data_[pos_++] << 8) |
                (data_[pos_++] << 0));
        }
        //
        public void skip(int skipByte)
        {
            pos_ += skipByte;
        }
        public bool finished()
        {
            return pos_ >= data_.Length;
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public class AlarmEventArgs 
        : EventArgs
    {
        public SocketStream ms = null;
        public AlarmEventArgs(SocketStream ss)
        {
            this.ms = ss;
        }
    }

    /// <summary>
    /// 
    /// </summary>
    class Socket
    {
        //
        private System.Net.Sockets.NetworkStream networkStream = null;
        private System.Net.Sockets.TcpClient client = null;
        private System.Net.Sockets.TcpListener listener = null;
        private Thread pumpThread = null;
        private bool exitFlag = false;

        //
        public delegate void OnArriveData(object sender, AlarmEventArgs e);
        public event OnArriveData onArriveData;

        //
        public void listen(string ipAddr, int portNo)
        {
            //
            close();
            this.exitFlag = false;

            //ListenするIPアドレス
            System.Net.IPAddress ipAdd = System.Net.IPAddress.Parse(ipAddr);

            //TcpListenerオブジェクトを作成する
            listener = new System.Net.Sockets.TcpListener(ipAdd, portNo);

            // スレッドを起動
            this.pumpThread = new Thread(new ThreadStart(pumpMain));
            this.pumpThread.Start();
        }
        /// <summary>
        /// 
        /// </summary>
        private void connect()
        {
            //Listenを開始する
            listener.Start();
            Debug.WriteLine("Start listen...({0}:{1})。",
                ((System.Net.IPEndPoint)listener.LocalEndpoint).Address,
                ((System.Net.IPEndPoint)listener.LocalEndpoint).Port);

            /*
             * AcceptTcpClient()はブロッキングしてしまうため
             * 途中で終了要求があった時に何もできなくなってしまうため
             * 前にPending()を挟む
             */
            //接続要求があったら受け入れる
            while (!listener.Pending())
            {
                // 100ms毎にトライ
                Thread.Sleep(100);
                // 終了フラグが来ていたら終了
                if (this.exitFlag)
                {
                    return;
                }
            }
            this.client = listener.AcceptTcpClient();
            Debug.WriteLine("クライアント({0}:{1})と接続しました。",
                ((System.Net.IPEndPoint)client.Client.RemoteEndPoint).Address,
                ((System.Net.IPEndPoint)client.Client.RemoteEndPoint).Port);

            //NetworkStreamを取得
            this.networkStream = this.client.GetStream();

            // タイムアウトは1秒
            this.networkStream.ReadTimeout = System.Threading.Timeout.Infinite;
            this.networkStream.WriteTimeout = System.Threading.Timeout.Infinite;
        }
        //
        private void pumpMain()
        {
            //
            connect();
            //
            for (;;)
            {
                // 終了フラグが来ていたら終了
                if (this.exitFlag)
                {
                    break;
                }
                //クライアントから送られたデータを受信する
                bool isReadHeader = false;
                int dataSize = 0;
                Queue<byte> reciveData = new Queue<byte>();
                //MemoryStream ms = new MemoryStream();
                byte[] resBytes = new byte[256];
                int resSize = 0;
                do
                {
                    //データの一部を受信する
                    try
                    {
                        resSize = networkStream.Read(resBytes, 0, resBytes.Length);
                    }
                    catch(IOException )
                    {
                        Debug.WriteLine("IOE");
                        break;
                    }
                    // 
                    if (resSize <= 0)
                    {
                        break;
                    }
                    //
                    for (int i = 0; i < resSize; ++i)
                    {
                        reciveData.Enqueue(resBytes[i]);
                    }
                } while (networkStream.DataAvailable);
                // データが来てなおかつヘッダ分のデータが来ていればイベント発生
                if (resSize > 0)
                {
                    // ヘッダが読まれていなければヘッダを読む
                    if(!isReadHeader)
                    {
                        if (reciveData.Dequeue() != 0x12 ||
                            reciveData.Dequeue() != 0x34)
                        {
                            Console.WriteLine("Clear Data");
                            reciveData.Clear();
                        }
                        dataSize = 
                            ((int)(reciveData.Dequeue() << 8) |
                            ((int)reciveData.Dequeue() << 0));
                        isReadHeader = true;
                        Console.WriteLine("ReadHeader {0}byte", dataSize );
                    }
                    // ヘッダに記載されているサイズを超えたら転送
                    if(isReadHeader && 
                        (dataSize - 4 <= reciveData.Count))
                    {
                        Console.WriteLine("TranserToApp {0}({1})byte", dataSize, reciveData.Count);
                        var ss = new SocketStream(reciveData.ToArray());
                        onArriveData(this, new AlarmEventArgs(ss));
                        isReadHeader = false;
                    }
                }
                else
                {
                    // 接続しなおす
                    Debug.WriteLine("Disconnected");
                    connect();
                }

                //
                Thread.Sleep(10);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public void close()
        {
            // 終了を設定
            this.exitFlag = true;
            // HACK: 強制的に止める
            if (this.pumpThread != null)
            {
                this.pumpThread.Abort();
            }
            // 閉じる
            if (this.networkStream != null)
            {
                this.networkStream.Close();
            }
            if (this.client != null)
            {
                this.client.Close();
                Debug.WriteLine("クライアントとの接続を閉じました。");
            }
            //リスナを閉じる
            if (this.listener != null)
            {
                this.listener.Stop();
                Debug.WriteLine("Listenerを閉じました。");
            }
        }
    }
}
