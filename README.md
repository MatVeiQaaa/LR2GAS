# LR2GAS
An injected .dll to add "Gauge Auto Shift" feature for Lunatic Rave 2.

It works by calculating "Easy", "Groove" and "Hard" gauge types simultaniously during gameplay and by tracking current gauge value in game memory. According to conditions it replaces the values in game memory with those from hook memory. Also restores the gauge type to the one you started with after the score is saved, so you can start next chart with same gauge type, and displays according graph for the gauge type you finished with.

NOTE: Doesn't support courses and gauges that arent EASY, GROOVE or HARD yet.

Kudos to https://github.com/SayakaIsBaka for great help in solving a few issues and for testing on Windows 10.
