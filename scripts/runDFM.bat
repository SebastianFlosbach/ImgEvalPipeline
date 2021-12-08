@echo off

mkdir ImageGraph
del /S /Q ImageGraph

set startTime=%time%
bin\\Preprocessing.exe
echo Start Time: %startTime%
echo End Time: %time%