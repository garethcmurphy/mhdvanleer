#!/usr/bin/env python3
import h5py

files = glob.glob('hdf5_out_2d_*.h5')
f = h5py.File('hdf5_out_2d_00000.h5', 'r')

print (f['Bx'])
import matplotlib.pyplot as plt


def printname(name):
    print(name)


f.visit(printname)
bx = f['Bx']
plt.figure()
ax = plt.subplot(2, 3, 1)
im = ax.pcolor(f['Velx'])
ax = plt.subplot(2, 3, 2)
im = ax.pcolor(f['Vely'])
ax = plt.subplot(2, 3, 3)
im = ax.pcolor(f['Bx'])
ax = plt.subplot(2, 3, 4)
im = ax.pcolor(f['By'])
ax = plt.subplot(2, 3, 5)
im = ax.pcolor(f['Density'])
ax = plt.subplot(2, 3, 6)
im = ax.pcolor(f['Energy'])
plt.savefig('test_output.png', dpi=100)
plt.show()
