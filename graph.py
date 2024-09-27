import matplotlib.pyplot as plt
import numpy as np

# Data
categories = ['Sequential File', 'AVL File', 'Extendible Hashing']

# Creation Data
creation_time = [0.973, 6.128, 11.6744]
creation_accesses = [100000, 1668080, 1613563]

# Search Data
search_time = [0.075, 0.065, 0.043]
search_accesses = [19, 17, 2]

# Insertion Data
insertion_time = [0.037, 696, 0.091]
insertion_accesses = [1, 199407, 3]

# Deletion Data
deletion_time = [0.071, 704, 0.13]
deletion_accesses = [1, 199422, 3]

# Subplots
fig, axs = plt.subplots(4, 2, figsize=(12, 10))
fig.suptitle('Performance Comparison (Time in ms and Number of Accesses)')

# Plot Creation
axs[0, 0].bar(categories, creation_time, color='royalblue')
axs[0, 0].set_title('Creation Time (s)')
axs[0, 0].set_ylabel('Time (s)')

axs[0, 1].bar(categories, creation_accesses, color='seagreen')
axs[0, 1].set_title('Creation Accesses')
axs[0, 1].set_ylabel('N. Accesses')

# Plot Search
axs[1, 0].bar(categories, search_time, color='royalblue')
axs[1, 0].set_title('Search Time (ms)')
axs[1, 0].set_ylabel('Time (ms)')

axs[1, 1].bar(categories, search_accesses, color='seagreen')
axs[1, 1].set_title('Search Accesses')
axs[1, 1].set_ylabel('N. Accesses')

# Plot insertion
axs[2, 0].bar(categories, insertion_time, color='royalblue')
axs[2, 0].set_title('Insertion Time (ms)')
axs[2, 0].set_ylabel('Time (ms)')
# use log scale
axs[2, 0].set_yscale('log')
axs[2, 0].set_ylim([0.01, 1000])

axs[2, 1].bar(categories, insertion_accesses, color='seagreen')
axs[2, 1].set_title('Insertion Accesses')
axs[2, 1].set_ylabel('N. Accesses')
# use log set
axs[2, 1].set_yscale('log')
axs[2, 1].set_ylim([0.01, 1000000])

# Plot deletion
axs[3, 0].bar(categories, deletion_time, color='royalblue')
axs[3, 0].set_title('Deletion Time (ms)')
axs[3, 0].set_ylabel('Time (ms)')
# use log scale
axs[3, 0].set_yscale('log')
axs[3, 0].set_ylim([0.01, 1000])

axs[3, 1].bar(categories, deletion_accesses, color='seagreen')
axs[3, 1].set_title('Deletion Accesses')
axs[3, 1].set_ylabel('N. Accesses')
# use log scale
axs[3, 1].set_yscale('log')
axs[3, 1].set_ylim([0.01, 1000000])

# Adjust layout
plt.tight_layout(rect=[0, 0.03, 1, 0.95])
plt.show()
