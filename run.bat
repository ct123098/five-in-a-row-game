
@echo off

g++ TongsAI.cpp -o TongsAI.exe

::	set player1="CON"
::	set player1="randput.exe"
::	set player1="seqput.exe"
	set player1="TongsAI.exe"


::	set player2="CON"
	set player2="randput.exe"
::	set player2="seqput.exe"
::	set player2="TongsAI.exe"


GameRunner.exe %player1% %player2%

@echo on