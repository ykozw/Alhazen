# **_Alhazen_ Render**

![rt3_resize.png](/asset/home/top.png?raw=true)

# 概要

Alhazenは全ての光学現象(フレア、ゴースト、色収差、ボケ)を理論的に正しく実装することを目標に作られています。

![lens_resize.png](/asset/home/lens.png?raw=true)

# ビルド
## Mac
```
brew install tbb
git clone https://github.com/qatnonoil/Alhazen.git
cd Alhazen
git submodule update --init
premake5 xcode4
cd generated
xcodebuild -configuration Release
```

## Windows
TODO: 書く

# Alhazen

_[Book of Optics](https://en.wikipedia.org/wiki/Book_of_Optics)_ 著者の _[Ibn al-Haitham](https://en.wikipedia.org/wiki/Alhazen)_ は _"Alhazen"_ と呼ばれています。

![alhazen.gif](/asset/home/alhazen.gif?raw=true)

# その他
* [レイトレ合宿3!!!](https://sites.google.com/site/raytracingcamp3/)にて2位を頂きました。([slide](http://www.slideshare.net/qatnonoil/alhazen-render3-52216918)).
* [レイトレ合宿4!?](https://sites.google.com/site/raytracingcamp4/) にて12位を頂きました。
* [レイトレ合宿5](https://sites.google.com/site/raytracingcamp5/) にて12位を頂きました。
