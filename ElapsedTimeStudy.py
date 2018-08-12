from ROOT import TTree
from ROOT import TFile
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import seaborn as sns
import pandas as pd
import numpy as np


material_palette = ["#4CAF50", "#2196F3", "#9E9E9E", "#FF9800", "#607D8B", "#9C27B0"]
sns.set_palette(material_palette)
sns.set(style='ticks', palette='Spectral', font_scale=1.5)

nAPA = range(1,13)
infile_stable = []
intree_stable = []

infile_sort = []
intree_sort = []

timing_timesort_df=pd.DataFrame()
timings_timesort = []
Sort_timesort    = []
APAs_timesort    = []

timing_spacesort_df=pd.DataFrame()
timings_spacesort = []
Sort_spacesort    = []
APAs_spacesort    = []

timing_clustering_df=pd.DataFrame()
timings_clustering = []
Sort_clustering    = []
APAs_clustering    = []
Config = 5
for i in range(0,len(nAPA)):
    print("Considering APA"+str(nAPA[i]))
    infile_stable.append(TFile("build/OutputPDS_time_"+str(nAPA[i])+"APA_1Sort.root","READ"))
    intree_stable.append(infile_stable[i].Get("TimingInfo"))
    for entry in intree_stable[i]:
        if entry.Config is not Config:
            continue
        totaltime=0.
        for time in entry.TimeOrdering_WireClustTime:
            totaltime = totaltime+time
        timings_timesort.append(totaltime*1000.)
        APAs_timesort   .append(nAPA[i])
        Sort_timesort   .append("std::stable_sort")

        totaltime=0.
        for time in entry.SpaceOrdering_WireClustTime:
            totaltime = totaltime+time
        timings_spacesort.append(totaltime*1000.)
        APAs_spacesort   .append(nAPA[i])
        Sort_spacesort   .append("std::stable_sort")

        totaltime=0.
        for time in entry.Clustering_WireClustTime:
            totaltime = totaltime+time
        timings_clustering.append(totaltime*1000.)
        APAs_clustering   .append(nAPA[i])
        Sort_clustering   .append("std::stable_sort")

    infile_sort.append(TFile("build/OutputPDS_time_"+str(nAPA[i])+"APA_0Sort.root","READ"))
    intree_sort.append(infile_sort[i].Get("TimingInfo"))
    for entry in intree_sort[i]:
        if entry.Config is not Config:
            continue
        totaltime=0.
        for time in entry.TimeOrdering_WireClustTime:
            totaltime = totaltime+time
        timings_timesort.append(totaltime*1000.)
        APAs_timesort   .append(nAPA[i])
        Sort_timesort   .append("std::sort")

        totaltime=0.
        for time in entry.SpaceOrdering_WireClustTime:
            totaltime = totaltime+time
        timings_spacesort.append(totaltime*1000.)
        APAs_spacesort   .append(nAPA[i])
        Sort_spacesort   .append("std::sort")

        totaltime=0.
        for time in entry.Clustering_WireClustTime:
            totaltime = totaltime+time
        timings_clustering.append(totaltime*1000.)
        APAs_clustering   .append(nAPA[i])
        Sort_clustering   .append("std::sort")

        
timing_timesort_df['APA']                 = APAs_timesort
timing_timesort_df['Time sorting Time']   = np.float32(timings_timesort)
timing_timesort_df['Sort algorithm']      = Sort_timesort

timing_spacesort_df['APA']                = APAs_spacesort
timing_spacesort_df['Space sorting Time'] = np.float32(timings_spacesort)
timing_spacesort_df['Sort algorithm']     = Sort_spacesort

timing_clustering_df['APA']               = APAs_clustering
timing_clustering_df['Clustering Time']   = np.float32(timings_clustering)
timing_clustering_df['Sort algorithm']    = Sort_clustering

print(timing_timesort_df  .describe())
print(timing_spacesort_df .describe())
print(timing_clustering_df.describe())

fig, ax = plt.subplots(figsize=(20, 12))
fig.suptitle("Timing")
ax = sns.boxplot(data=timing_clustering_df, x="APA", y="Clustering Time", hue="Sort algorithm",whis=[1.,99.])
ax.set_xlabel('number of APAs')
ax.set_ylabel('Time [ms]')
ax.yaxis.set_major_locator(ticker.MultipleLocator(0.1))
ax.yaxis.set_major_formatter(ticker.ScalarFormatter())
plt.grid(True)
fig.savefig("Clustering_timeplot.pdf")


fig, ax = plt.subplots(figsize=(20, 12))
fig.suptitle("Timing")
ax = sns.boxplot(data=timing_timesort_df, x="APA", y="Time sorting Time", hue="Sort algorithm",whis=[1.,99.])
ax.set_xlabel('number of APAs')
ax.set_ylabel('Time [ms]')
ax.yaxis.set_major_locator(ticker.MultipleLocator(0.1))
ax.yaxis.set_major_formatter(ticker.ScalarFormatter())
plt.grid(True)
fig.savefig("Time_sorting_timeplot.pdf")


fig, ax = plt.subplots(figsize=(20, 12))
fig.suptitle("Timing")
ax = sns.boxplot(data=timing_spacesort_df, x="APA", y="Space sorting Time", hue="Sort algorithm",whis=[1.,99.])
ax.set_xlabel('number of APAs')
ax.set_ylabel('Time [ms]')
ax.yaxis.set_major_locator(ticker.MultipleLocator(0.1))
ax.yaxis.set_major_formatter(ticker.ScalarFormatter())
plt.grid(True)
fig.savefig("Space_sorting_timeplot.pdf")
