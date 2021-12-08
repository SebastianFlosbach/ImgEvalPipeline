@echo off

call runDFM
call runDFMToAliceVision
call runStructureFromMotion dfm
call readPoses dfm
