import matplotlib.pyplot as plt
plt.subplot(331).set_ylim([0, 2.5])
plt.plot([1,2,4,8,16], [1.00045, 0.835091, 0.666233, 0.576068, 0.548894], 'ro-', [1], [0.571738], 'go')
plt.ylabel('Power Log Graph')
plt.subplot(332).set_ylim([0, 2.5])
plt.plot([1,2,4,8,16], [1.20927, 0.955278, 0.718347, 0.595529, 0.601904], 'ro-', [1], [0.49394], 'go')
plt.subplot(333).set_ylim([0, 2.5])
plt.plot([1,2,4,8,16], [2.4458, 1.61712, 1.0962, 0.815608, 0.84018], 'ro-', [1], [0.492658], 'go')
plt.subplot(334).set_ylim([0, 22])
plt.plot([1,2,4,8,16], [2.51151, 2.32082, 1.86214, 1.49956, 1.41741], 'ro-', [1], [0.343193], 'go')
plt.ylabel('1000x1000 grid')
plt.subplot(335).set_ylim([0, 22])
plt.plot([1,2,4,8,16], [6.15369, 4.9619, 4.27494, 3.25356, 3.06569], 'ro-', [1], [0.279549], 'go')
plt.subplot(336).set_ylim([0, 22])
plt.plot([1,2,4,8,16], [21.9111, 16.7763, 10.0067, 7.4294, 6.83113], 'ro-', [1], [0.276739], 'go')
plt.subplot(337).set_ylim([0, 1.4])
plt.plot([1,2,4,8,16], [1.05596, 0.623712, 0.364972, 0.26342, 0.233301], 'ro-', [1], [1.25175], 'go')
plt.xlabel('B=0.5')
plt.ylabel('500 cliques')
plt.subplot(338).set_ylim([0, 1.4])
plt.plot([1,2,4,8,16], [1.19634, 0.724163, 0.437261, 0.284439, 0.274892], 'ro-', [1], [1.31954], 'go')
plt.xlabel('B=0.05')
plt.subplot(339).set_ylim([0, 1.4])
plt.plot([1,2,4,8,16], [1.36096, 0.885213, 0.5429, 0.356262, 0.352279], 'ro-', [1], [1.23901], 'go')
plt.xlabel('B=0.005')
plt.show()

plt.plot([1,2,4,8,16], [1.19634/1.19634, 1.19634/0.724163, 1.19634/0.437261, 1.19634/0.284439, 1.19634/0.274892], 'ro-')
plt.xscale('log')
plt.title('Speedup of B=0.05, Cliques, Logarithmic scale')
plt.xlabel('log # threads')
plt.ylabel('sec')
plt.show()