@echo off
cls
echo  ===compiling vertex shader=== 

glslc .\vert.vert -o vert.spv


echo  ===compiling fragment shader===
glslc .\frag.frag -o frag.spv
