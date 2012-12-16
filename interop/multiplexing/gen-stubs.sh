#!/bin/bash

mkdir -p stubs
cd stubs 
idl --no-paths --poa ../idl/hello.idl
