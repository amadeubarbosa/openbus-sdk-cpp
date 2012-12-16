#!/bin/bash

mkdir -p stubs
cd stubs 
idl --no-paths --poa --typecode ../idl/messages.idl
