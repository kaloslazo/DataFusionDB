#!/bin/bash

python setup.py build_ext --inplace;
mv link_connection.*.so frontend/;
