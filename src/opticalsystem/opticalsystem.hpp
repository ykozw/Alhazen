#pragma once

#include "../pch.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class OpticalSystem AL_FINAL {
 private:
  class Impl;
  std::unique_ptr<Impl> pimpl_;

 public:
  OpticalSystem() = default;
  ~OpticalSystem() = default;
  // 初期化関連
  bool setupFromOtx(const char* fileName);
  void setupFromScratch();

  // 取得関連
  float fStop() const;
  float focalLength() const;
  float lensPower() const;

  // 設定関連
  void autoFocus();  // フォーカス対象を投げる？失敗したときに何かを返すとか。

  // トレース関連
  void trace();  // 単波長版と複数の波長を同時に扱うのを二つ用意する
 private:
};
