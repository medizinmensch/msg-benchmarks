import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import re, os

# raw = open("results.csv", "r")
os.remove("results_clean.csv") 
clean_file = open("results_clean.csv", "w")
clean_file.write('Tag;client_id;msg_size;repetitions;client_count;Throughput_in_KiBi/second;bytes_received;delta_us\n')

with open ("results.txt", 'r' ) as f:
    content = f.read()
    new = re.sub('^(?![GE]).*\n', '', content,0, re.MULTILINE)
    clean_file.write(new)

clean_file.close()

df = pd.read_csv('results_clean.csv', sep=";")

table = pd.pivot_table(df, values='Throughput_in_KiBi/second', index=['msg_size'], columns=['client_count'], aggfunc=np.median)

fig, ax = plt.subplots()



ax.plot(t, s)



print(table)