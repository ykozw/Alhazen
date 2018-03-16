#pragma once

#include "pch.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
struct ArgConfig {
 public:
  //
  bool dumpLensData = false;
  bool floatException = false;
  bool unitTest = false;
  //
  std::string sceneFilePath;
  std::string baseFileDir;
  std::string sceneFileName;
};

/*
-------------------------------------------------
単純なアプリケーションクラス
初期化と終了処理だけ入っている
-------------------------------------------------
*/
class App {
 public:
  App() = default;
  ~App() = default;
  int32_t run(int32_t argc, char* argv[]);

 protected:
  virtual int32_t runApp(const ArgConfig& config) = 0;
};
