#!/usr/bin/env python
# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>

# ------------------------------------------------------------------------
# Filename   : heatmap.py
# Date       : 2013-04-19
# Updated    : 2014-01-04
# Author     : @LotzJoe >> Joe Lotz
# Description: My attempt at reproducing the FlowingData graphic in Python
# Source     : http://flowingdata.com/2010/01/21/how-to-make-a-heatmap-a-quick-and-easy-solution/
#
# Other Links:
#     http://stackoverflow.com/questions/14391959/heatmap-in-matplotlib-with-pcolor
#
# ------------------------------------------------------------------------

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# page = urlopen("http://datasets.flowingdata.com/ppg2008.csv")
# forget the diretc down loading. DOwnload it before hand and read like normal file
page = "ppg2008.csv"
# read_csv will give us a standard pandas dataframe
# it's assumed first row will be header.
nba = pd.read_csv(page, index_col=0)

# it's assumed as is typical that rows are observation, samples etc 
# and columns are the different variables
# the describe method will give us summary statistics of the variables i.e rows are "reduced".
# d=nba.describe()
# in fact all methods will reduce the rows based on the operation
# nba.max() max of each column, etc.

# nba_normed = (nba - nba.mean()) / (nba.max() - nba.min())
