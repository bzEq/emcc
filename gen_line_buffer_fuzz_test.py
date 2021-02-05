#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import random
import time

random.seed(int(time.time()))

MaxWidth = 1 << 16
MaxHeight = 1 << 20


def DoErase(f):
    line = int(random.random() * MaxHeight)
    col = 0
    length = int(random.random() * MaxWidth)
    f.write("e %d %d %d\n" % (line, col, length))


def DoInsert(f):
    line = int(random.random() * MaxHeight)
    col = 0
    length = int(random.random() * MaxWidth)
    f.write("i %d %d %d\n" % (line, col, length))


def DoAppend(f):
    length = int(random.random() * MaxWidth)
    f.write("a %d\n" % length)

os.makedirs('fuzz_tests', exist_ok=True)
with open('fuzz_tests/line_buffer.test', 'w') as f:
    for i in range(1 << 2):
        dice = random.random()
        if dice < 0.6:
            DoErase(f)
        elif dice < 0.8:
            DoInsert(f)
        else:
            DoAppend(f)
