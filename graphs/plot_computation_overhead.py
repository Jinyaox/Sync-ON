import numpy as np
import matplotlib.pyplot as plt

# Data for each protocol
protocols = [
    'Glocker 17',
    'Parameswarath 22',
    'Parameswarath 24'
]

# Computation times for key fob and car (in ms)
key_fob_times = [5.58, 0.9598, 0.83]
car_times = [4.65, 0.9798, 0.34]

# Set up the bar positions
x = np.arange(2)
width = 0.07  # Width of the bars
n_bars = len(protocols)

# Create figure and axis with higher DPI for better SVG quality
plt.figure(figsize=(4,4), dpi=300)

# Define colors similar to the original graph
colors = ['#fd8d3c', '#756bb1', '#74c476']

# Create bars for each protocol
for i in range(n_bars):
    offset = (i - n_bars/2 + 0.5) * width
    plt.bar(x + offset, [key_fob_times[i], car_times[i]], width,
            label=protocols[i], color=colors[i])

# Customize the plot
plt.ylabel('Computation Time (ms)')
plt.title('Time taken during authentication')
plt.xticks(x, ['Key fob', 'Car'])
plt.legend()

# Set y-axis limit to match the original graph
plt.ylim(0, 6)

# Add grid lines (optional)
plt.grid(axis='y', linestyle='--', alpha=0.7)

# Adjust layout to prevent label cutoff
plt.tight_layout()

# Save as SVG
plt.savefig('authentication_times.svg', format='svg', bbox_inches='tight')

# Save as PNG as well (as a backup)
plt.savefig('authentication_times.png', dpi=300, bbox_inches='tight')

# Show the plot
plt.show()