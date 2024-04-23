#script for converting .dat & .wav files to plots: FFT, SNR, real-time plots

#import packages
import os
import tkinter as tk
from tkinter import filedialog
import numpy as np
import scipy.io.wavfile as sc
import matplotlib.pyplot as plt
from scipy.io import wavfile


#take input from the user.
def choose_and_read_file():
    #window for search
    root = tk.Tk()
    root.withdraw()

    #file choose
    file_path = filedialog.askopenfilename(title="Choose a file")

    #find file expension
    file_extension = os.path.splitext(file_path)[1]

    #convert to wav if needed
    if file_extension.lower() == '.wav':
        sample_rate, data = wavfile.read(file_path)
    elif file_extension.lower() == '.dat':
        dat_file = np.loadtxt(file_path, skiprows=2)
        output_path = os.path.splitext(file_path)[0] + ".wav"
        sc.write(output_path, 44100, dat_file)
        sample_rate, data = wavfile.read(output_path)
    else:
        print("Can only handle .dat or .wav files")
        return 'unknown'

    return sample_rate, data


#plot the FFT
def plot_fft(data, sample_rate):
    #calc fft
    fft_result = np.fft.fft(data[:, 1])

    #find freqs
    frequencies = np.fft.fftfreq(len(data), 1/sample_rate)

    #plot
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(frequencies[:len(frequencies)//2], np.abs(fft_result)[:len(frequencies)//2])
    ax.set_title("FFT of WAV file")
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Amplitude")
    ax.grid(True)

    return fig
    


#plot the SNR
def plot_snr(data, samples_noise):
    data = np.array(data)

    #noise ref
    noise_ref = np.mean(data[:samples_noise, 1]) ** 2

    #snr list
    #snr_db = []
    snr_db = np.zeros(len(data))

    #sample by sample snr based on single noise value from defined sample number at beginning
    for i in range(len(data)):
        #signal power for the current sample
        signal_power = data[i, 1] ** 2

        #SNR in dB for the current sample
        if noise_ref == 0:
            snr_db[i] = -np.inf
        else:
            snr_db[i] = 10 * np.log10(signal_power / noise_ref)

    #time values
    time = data[:, 0]

    return time, snr_db

#plot the real-time pre-and post filter
def plot_time_domain(data, sample_rate):
    time = data[:, 0]  #times
    amplitude = data[:, 1]  #amplitudes

    #plot time domain signal
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(time, amplitude)
    ax.set_title("Time Domain Plot")
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Amplitude")
    ax.grid(True)

    return fig


#main code
print("choose BEFORE .dat file")
sample_rate, data = choose_and_read_file()
print("choose AFTER .dat file")
sample_rate2, data2 = choose_and_read_file()

#plot it all
fft_fig1 = plot_fft(data, sample_rate)
fft_fig2 = plot_fft(data2, sample_rate2)
time_domain_fig1 = plot_time_domain(data, sample_rate)
time_domain_fig2 = plot_time_domain(data2, sample_rate2)

#before SNR with time
time, snr_db = plot_snr(data, 2000)
fig_snr, ax_snr = plt.subplots(figsize=(10, 6))
ax_snr.plot(time, snr_db)
ax_snr.set_title("Signal-to-Noise Ratio (SNR) vs Time")
ax_snr.set_xlabel("Time (s)")
ax_snr.set_ylabel("SNR (dB)")
ax_snr.grid(True)

#after SNR with time
time2, snr_db2 = plot_snr(data2, 2000)
fig_snr2, ax_snr2 = plt.subplots(figsize=(10, 6))
ax_snr2.plot(time2, snr_db2)
ax_snr2.set_title("Signal-to-Noise Ratio (SNR) vs Time")
ax_snr2.set_xlabel("Time (s)")
ax_snr2.set_ylabel("SNR (dB)")
ax_snr2.grid(True)


plt.show()