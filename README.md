# Multithread Design

## Description
In this project, we analyze the collected data of the specifications and prices of the houses. We classify the data using labeling, calculating the average and standard deviation, and then determine the price range of each house. We obtain the accuracy by comparing the determined price range with the actual price value and performing a series of calculations.

## Implementation
We implement this project in two ways, serial an parallel. In parallel implementation, we parallelize the hotspot operations (functions which are the most time-consuming in the program). On the other words, we divide the data into smaller groups and perform calculations in these groups in parallel.
