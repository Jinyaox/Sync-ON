import numpy as np
import matplotlib.pyplot as plt

# Data for each scenario
scenarios = ['Fob', 'Car', 'Car with Correction']

# Computation times (in ms)
times = [2.850845, 3.893558, 3.962788]

# Set up the bar positions
x = np.arange(len(scenarios))
width = 0.2  # Width of the bars
# Create figure and axis
plt.figure(figsize=(4,4))

# Create bars
plt.bar(x, times, width, color=['#3182bd', '#e6550d', '#756bb1'])

# Customize the plot
plt.ylabel('Computation Time (ms)')
plt.title('Time taken during authentication')
plt.xticks(x, scenarios)

# Set y-axis limit to match the original graph
plt.ylim(0, 6)

# Add grid lines (optional)
plt.grid(axis='y', linestyle='--', alpha=0.7)

# Adjust layout to prevent label cutoff
plt.tight_layout()

# Save as SVG
plt.savefig('proposed_protocol_times.svg', format='svg', bbox_inches='tight')
# Save as PNG as well (as a backup)
plt.savefig('proposed_protocol_times.png', dpi=300, bbox_inches='tight')

# Show the plot
plt.show()