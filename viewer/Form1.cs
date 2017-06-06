using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using alhzen_viewer;
using System.Diagnostics;
using System.IO;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Threading;

namespace alhzen_viewer
{
    /// <summary>
    /// 
    /// </summary>
    public partial class Form1 : Form
    {
        //
        private Socket socket = null;
        //
        public Form1()
        {
            InitializeComponent();
            this.TopMost = true;
            this.socket = new Socket();
            this.imageDatas = new List<ImageData>();
            //
            myTimer.Tick += new EventHandler(TimerEventProcessor);
            myTimer.Interval = 500;
            myTimer.Start();
            //
            updateTitleBar();
        }
        ~Form1()
        {
        }

        private void TimerEventProcessor(Object myObject,
                                           EventArgs myEventArgs)
        {
            this.cpuUsage = pc.NextValue();
            //
            updateTitleBar();
        }

        private void ファイルToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void 設定ToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            config f = new config();
            f.ShowDialog(this);
            f.Dispose();
        }

        private void 開始ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.socket.onArriveData += onRecieveData;
            // HACK: IP/Portは適当
            this.socket.listen("127.0.0.1", 2001);
        }

        /// <summary>
        /// データが来た時呼ばれる
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void onRecieveData(object sender, AlarmEventArgs e)
        {
            this.Invoke(new onRecieveDataMainDelegate(onRecieveDataMain), e.ms);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="ms"></param>
        delegate void onRecieveDataMainDelegate(SocketStream ms);
        void onRecieveDataMain(SocketStream ss)
        {
            while (!ss.finished())
            {
                switch (ss.popUint8())
                {
                    case 0:
                        onResizeImage(ss);
                        break;
                    case 1:
                        onArrivePixelData(ss);
                        break;
                    default:
                        break;
                }
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ms"></param>
        private void onResizeImage(SocketStream ss)
        {
            // img領域をリサイズする
            int w = (int)ss.popUint32();
            int h = (int)ss.popUint32();
            int tileSize = (int)ss.popUint32();
            this.pictureBox1.Image = new Bitmap(w, h);
            this.pictureBox1.Anchor = AnchorStyles.Left | AnchorStyles.Top;
            // イメージをクリアする
            clearImage(tileSize);
            // リサイズ
            this.Size = this.pictureBox1.PreferredSize + this.PreferredSize;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="ms"></param>
        private void onArrivePixelData(SocketStream ss)
        {
            // 範囲を取得
            var img = this.pictureBox1.Image;
            int sx = (int)ss.popUint32();
            int sy = (int)ss.popUint32();
            int w = (int)ss.popUint32();
            int h = (int)ss.popUint32();
            //
            sy = img.Height - sy - h;
            // タイルに書き込む
            Bitmap tile = new Bitmap(w, h, PixelFormat.Format24bppRgb);
            BitmapData bmpData = tile.LockBits(new Rectangle(0, 0, w, h), ImageLockMode.WriteOnly, PixelFormat.Format24bppRgb);
            int tileSizeInBytes = w * h * 3;
            bool reciveOkay = (ss.data_.Count() > (ss.pos_ + tileSizeInBytes));
            Marshal.Copy(ss.data_, ss.pos_, bmpData.Scan0, tileSizeInBytes);
            tile.UnlockBits(bmpData);
            tile.RotateFlip(RotateFlipType.RotateNoneFlipY);
            ss.skip(tileSizeInBytes);
            //
            Graphics g = Graphics.FromImage(img);
            g.DrawImage(tile, new Rectangle(sx, sy, w, h), new Rectangle(0, 0, w, h), GraphicsUnit.Pixel);
            //
            this.pictureBox1.Refresh();
            g.Dispose();
            tile.Dispose();
        }

        //
        private int delegateOnReciveData(int x, int y)
        {
            return 0;
        }

        private void 終了ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // アプリケーションの終了
            this.Close();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void クリアToolStripMenuItem_Click(object sender, EventArgs e)
        {
            clearImage(16);
        }

        void clearImage(int tileSize)
        {
            var img = this.pictureBox1.Image;
            int w = img.Width;
            int h = img.Height;
            var g = Graphics.FromImage(img);
            g.Clear(Color.Black);
            // TODO: フォトショップの透過色のようにする
            g.Clear(Color.White);
            // タイルを作成する
            // TODO: タイルの総数を算出
            int nx = w / tileSize + 1;
            int ny = h / tileSize + 1;
            int numTile = nx * ny;
            List<Rectangle> rs2 = new List<Rectangle>();
            for (int y = 0; y < h; y += tileSize)
            {
                for (int x = 0; x < w; x += tileSize)
                {
                    if ((x / tileSize + y / tileSize) % 2 == 0)
                    {
                        continue;
                    }
                    //
                    rs2.Add(new Rectangle(x, y, tileSize, tileSize));
                }
            }
            // 
            SolidBrush grayBrush = new SolidBrush(Color.FromArgb(200, 200, 200));
            g.FillRectangles(grayBrush, rs2.ToArray());
            //
            g.DrawLine(new Pen(Color.Red), 0, 0, w, h);
            g.DrawLine(new Pen(Color.Red), w, 0, 0, h);
            g.Dispose();
            //
            this.pictureBox1.Refresh();
        }

        // pngの保存ダイアログ
        private string getSaveDialogString ()
        {
            //SaveFileDialogクラスのインスタンスを作成
            SaveFileDialog sfd = new SaveFileDialog();

            //はじめのファイル名を指定する
            //はじめに「ファイル名」で表示される文字列を指定する
            sfd.FileName = "";
            //はじめに表示されるフォルダを指定する
            string stParentName = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            sfd.InitialDirectory = stParentName;
            //[ファイルの種類]に表示される選択肢を指定する
            //指定しない（空の文字列）の時は、現在のディレクトリが表示される
            sfd.Filter = "PNG(*.png)|*.png";
            //[ファイルの種類]ではじめに選択されるものを指定する
            //2番目の「すべてのファイル」が選択されているようにする
            sfd.FilterIndex = 2;
            //タイトルを設定する
            sfd.Title = "保存先のファイルを選択してください";
            //ダイアログボックスを閉じる前に現在のディレクトリを復元するようにする
            sfd.RestoreDirectory = true;
            //既に存在するファイル名を指定したとき警告する
            //デフォルトでTrueなので指定する必要はない
            sfd.OverwritePrompt = true;
            //存在しないパスが指定されたとき警告を表示する
            //デフォルトでTrueなので指定する必要はない
            sfd.CheckPathExists = true;

            //ダイアログを表示する
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                return sfd.FileName;
            }
            else
            {
                return "";
            }
            
        }

        private void 画像を保存ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string fileName = getSaveDialogString();
            if(fileName == "")
            {
                return;
            }
            // 現在のピクチャーボックスのイメージを保存
            var img = this.pictureBox1.Image;
            if(img ==null)
            {
                return;
            }
            img.Save(fileName, ImageFormat.Png);
        }

        private void フォルダを開くToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("EXPLORER.EXE", System.IO.Directory.GetCurrentDirectory());
        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            // 位置情報を保存
            curX = e.X;
            curY = e.Y;
            // ピクセル色を取得
            Bitmap bitmap = this.pictureBox1.Image as Bitmap;
            if(bitmap != null)
            {
                var col = bitmap.GetPixel(e.X, e.Y);
                curColor = col;
            }

            // タイトルバーの更新
            updateTitleBar();
        }

        private void 一時保存ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // TODO: 今のイメージを名前を付けて保存する
            string tag = Microsoft.VisualBasic.Interaction.InputBox("", "保存名", "", 100, 100);
            if(tag == "")
            {
                return;
            }
            //
            var newImageData = new ImageData();
            newImageData.name = tag;
            newImageData.image = this.pictureBox1.Image;
            imageDatas.Add(newImageData);
        }

        private void ToolStripMenuItem_Click_DeleteCurrentPreviewImage(object sender, EventArgs e)
        {
            // 現在のイメージのインデックスを削除
            if (previewImageIndex < imageDatas.Count)
            {
                imageDatas.RemoveAt(previewImageIndex);
            }
        }

        private void Form1_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            //受け取ったキーを表示する
            switch(e.KeyCode)
            {
                case Keys.Right:
                    ++previewImageIndex;
                    break;
                case Keys.Left:
                    --previewImageIndex;
                    break;
            }
            // ImageDataのサイズに合わせる
            previewImageIndex = Math.Min(previewImageIndex, imageDatas.Count() - 1);
            previewImageIndex = Math.Max(previewImageIndex, 0);
            // そのイメージに設定する
            if (imageDatas.Count != 0)
            {
                var id = imageDatas[previewImageIndex];
                this.pictureBox1.Image = id.image;
                imageName = id.name;
            }
            //
            updateTitleBar();
        }

        private void updateTitleBar()
        {
            this.Text = "AlhazenViewer";
            if(imageName != "")
            {
                this.Text += " Tag:" + imageName + "";
            }
            // ピクセル色
            if( curColor != null )
            {
                this.Text += " (" + curColor.R + "," + curColor.G + "," + curColor.B + ")";
            }
            // 位置
            this.Text += " (" + curX + "," + curY + ")";

            // CPU使用率
            this.Text += String.Format(" CPU:{0:f}%", this.cpuUsage);
        }

        System.Windows.Forms.Timer myTimer = new System.Windows.Forms.Timer();
        PerformanceCounter pc = new PerformanceCounter("Processor", "% Processor Time", "_Total", true);

        // タイトルバーに必要な情報
        string imageName = "";
        int curX;
        int curY;
        Color curColor;
        float cpuUsage = 0.0f;

        //
        struct ImageData
        {
            public string name;
            public Image image;
        }
        // 一時的なイメージのリスト
        List<ImageData> imageDatas;
        // 現在のプレビューのインデックス
        int previewImageIndex = 0;

        /// <summary>
        /// フォームが閉じるとき。
        /// 全ての終了処理はここにまとめる
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.socket.close();
        }
    }
}
