# 第 14 章 マルチスレッド
* 排他制御とデータ競合
* 標準ライブラリによるサポート 
    * std::thread
    * std::mutex
    * std::atomic
        * 一般的に専用のマシン命令を基に実装しているためmutexで保護するよりも効率性に優れる
    * std::future
    * std::promise
    * std::acync
    * std::packaged_task