#package import
import os
import tkinter as tk
from tkinter import filedialog
import numpy as np
import scipy.io.wavfile as sc
from scipy.io import wavfile
import plotly.graph_objects as go
import plotly.io as pio

op_val =0.7
col=1

#UI for selection of parameter to be compared
def select_parameter():
    #window creation
    root = tk.Tk()
    root.title("Select a Parameter to Compare")

    #variable storage for selection
    selected_option = tk.StringVar(root)

    #dropdown
    def select_option():
        selected_value = selected_option.get()
        root.quit()  # Quit the main loop
        return selected_value

    #options
    options = ["Learning_Rate", "Taps", "Layers", "Nonlinearity", "Error Backpropagation"]

    #default ption
    selected_option.set(options[0])

    #call dropdown
    dropdown = tk.OptionMenu(root, selected_option, *options)
    dropdown.pack()

    #yes button
    confirm_button = tk.Button(root, text="Select", command=select_option)
    confirm_button.pack()

    #run it
    root.mainloop()

    #close after selection
    root.destroy()

    return select_option()

#set names for plot naming and legend generation based on selection
def namesetter(selected_parameter):
    #init variable 
    plotname = ""
    dataname1 = ""
    dataname2 = ""
    dataname3 = ""
    dataname4 = ""
    dataname5 = ""

    #set legend names
    if selected_parameter == "Learning_Rate":
        plotname = "Comparison of Learning Rate"
        dataname1 = "LR=0.0025"
        dataname2 = "LR=0.005"
        dataname3 = "LR=0.01"
        dataname4 = "LR=0.025"
        dataname5 = "LR=0.05"
    elif selected_parameter == "Taps":
        plotname = "Comparison of Number of Taps"
        dataname1 = "Taps=100"
        dataname2 = "Taps=200"
        dataname3 = "Taps=300"
        dataname4 = "Taps=400"
        dataname5 = "Taps=500"
    elif selected_parameter == "Layers":
        plotname = "Comparison of Number of Layers"
        dataname1 = "nLayers=1"
        dataname2 = "nLayers=2"
        dataname3 = "nLayers=3"
        dataname4 = "nLayers=4"
        dataname5 = "nLayers=5"
    elif selected_parameter == "Nonlinearity":
        plotname = "Comparison of LMS,DNF, and Passthrough"
        dataname1 = "DNF"
        dataname2 = "LMS"
        dataname3 = "Pass-through"
        #dataname4 = "Nonlinearity=Tanh"
        # dataname5 = "void"
    elif selected_parameter == "Error Backpropagation":
        plotname = "Comparison of Error Backpropagation Frequencies"
        dataname1 = "Period=1"
        dataname2 = "Period=2"
        dataname3 = "Period=5"
        dataname4 = "Period=10"
        dataname5 = "Period=20"
    
    return plotname, dataname1, dataname2, dataname3, dataname4, dataname5
    


#choose and read audio files
def choose_and_read_file():
    #window to select files
    root = tk.Tk()
    root.withdraw()

    #file selection functionality
    file_path = filedialog.askopenfilename(title="Choose a file")

    #look for certain types
    file_extension = os.path.splitext(file_path)[1]

    #convert to wav
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

#FFT data get
def calculate_fft(data, sample_rate):
    #calculate
    fft_result = np.fft.fft(data[:, col])

    #x axis
    frequencies = np.fft.fftfreq(len(data), 1/sample_rate)

    #find magnitude
    magnitude_spectrum = np.abs(fft_result)[:len(frequencies)//2]

    #normalise
    normalised_magnitude_spectrum = magnitude_spectrum / np.max(magnitude_spectrum)

    return frequencies[:len(frequencies)//2], normalised_magnitude_spectrum

#SNR data get
def plot_snr(data, samples_noise, sample_rate):
    data = np.array(data)

    #noise ref from n samples
    noise_ref = np.mean(data[:samples_noise,col]) ** 2

    #SNR empties
    snr_db = np.zeros(len(data))

    #SNR sample by smaple: note I changed it to rolling mean but it was chaos. 
    for i in range(len(data)):
        #power of sample
        signal_power = data[i, col] ** 2

        #dB of sample
        if noise_ref == 0:
            snr_db[i] = -np.inf
        else:
            snr_db[i] = 10 * np.log10(signal_power / noise_ref)

    #time axis
    duration = len(data) / sample_rate
    time = np.linspace(0, duration, len(data))

    return time, snr_db

#real time plot of audio
def plot_time_domain(data, sample_rate):
    #axes data
    duration = len(data) / sample_rate
    time = np.linspace(0, duration, len(data))
    amplitude = data[:, col] 

    #normalise
    normalised_amplitude = amplitude / np.max(np.abs(amplitude))

    return time, normalised_amplitude


#main
selected_parameter = select_parameter()
plotname,dataname1, dataname2, dataname3, dataname4, dataname5 = namesetter(selected_parameter)
print("Choose file 1")
sample_rate, data = choose_and_read_file()
print("Choose file 2")
sample_rate2, data2 = choose_and_read_file()
print("Choose file 3")
sample_rate3, data3 = choose_and_read_file()
print("Choose file 4")
sample_rate4, data4 = choose_and_read_file()
print("Choose file 5")
sample_rate5, data5 = choose_and_read_file()

#FFT
frequencies, fft_result = calculate_fft(data, sample_rate)
frequencies2, fft_result2 = calculate_fft(data2, sample_rate2)
frequencies3, fft_result3 = calculate_fft(data3, sample_rate3)
frequencies4, fft_result4 = calculate_fft(data4, sample_rate4)
frequencies5, fft_result5 = calculate_fft(data5, sample_rate5)

#SNR
samples_snr = 2000 #can put in command line.
time_snr, snr_db = plot_snr(data, samples_snr, sample_rate)
time2_snr, snr_db2 = plot_snr(data2, samples_snr, sample_rate2)
time3_snr, snr_db3 = plot_snr(data3, samples_snr, sample_rate3)
time4_snr, snr_db4 = plot_snr(data4, samples_snr, sample_rate4)
time5_snr, snr_db5 = plot_snr(data5, samples_snr, sample_rate5)

#plot FFT
trace1 = go.Scatter(x=frequencies, y=fft_result, mode='lines',opacity = op_val, name=dataname1, visible=True)
trace2 = go.Scatter(x=frequencies2, y=fft_result2, mode='lines',opacity = op_val, name=dataname2, visible=True)
trace3 = go.Scatter(x=frequencies3, y=fft_result3, mode='lines',opacity = op_val, name=dataname3, visible=True)
trace4 = go.Scatter(x=frequencies4, y=fft_result4, mode='lines',opacity = op_val, name=dataname4, visible=True)
trace5 = go.Scatter(x=frequencies5, y=fft_result5, mode='lines',opacity = op_val, name=dataname5, visible=True)

#data list for fft
data_fft = [trace1, trace2, trace3, trace4, trace5]

#FFT plotly layout
layout_fft = go.Layout(
    title=plotname + " by FFT",
    title_x=0.5,
    xaxis=dict(title='Frequency [Hz]'),
    yaxis=dict(title='Amplitude'),
    showlegend=True
)


#FFT figure creation
fig_fft = go.Figure(data=data_fft, layout=layout_fft)

#save SVG: I know this is not ideal.
save_path_fft_svg= r"\\DESKTOP-HLBVG4T\geek\NINETT\Study\Glasgow University\5th Year\DST5\Project files\AIHearingAid\deepNeuronalFilter_optimised\audio_filter\filter_param_comparisons\\" + selected_parameter + "_fft_comparison.svg"
pio.write_image(fig_fft, save_path_fft_svg, format='svg')

#save html interactive plot.
save_path_fft = r"\\DESKTOP-HLBVG4T\geek\NINETT\Study\Glasgow University\5th Year\DST5\Project files\AIHearingAid\deepNeuronalFilter_optimised\audio_filter\filter_param_comparisons\\" + selected_parameter + "_fft_comparison.html"
fig_fft.write_html(save_path_fft)

#show plot
fig_fft.show()

#SNR plotting
trace_snr1 = go.Scatter(x=time_snr, y=snr_db, mode='lines',opacity = op_val, name=dataname1, visible=True)
trace_snr2 = go.Scatter(x=time2_snr, y=snr_db2, mode='lines',opacity = op_val, name=dataname2, visible=True)
trace_snr3 = go.Scatter(x=time3_snr, y=snr_db3, mode='lines',opacity = op_val, name=dataname3, visible=True)
trace_snr4 = go.Scatter(x=time4_snr, y=snr_db4, mode='lines',opacity = op_val, name=dataname4, visible=True)
trace_snr5 = go.Scatter(x=time5_snr, y=snr_db5, mode='lines',opacity = op_val, name=dataname5, visible=True)

#traces for SNR
data_snr = [trace_snr1, trace_snr2, trace_snr3, trace_snr4, trace_snr5]

#layout for SNR plot
layout_snr = go.Layout(
    title=plotname + ' by SNR',
    title_x=0.5,
    xaxis=dict(title='Time [s]'),
    yaxis=dict(title='SNR [dB]'),
    showlegend=True
)


#SNR fig
fig_snr = go.Figure(data=data_snr, layout=layout_snr)

#svg save
save_path_snr_svg = r"\\DESKTOP-HLBVG4T\geek\NINETT\Study\Glasgow University\5th Year\DST5\Project files\AIHearingAid\deepNeuronalFilter_optimised\audio_filter\filter_param_comparisons\\" + selected_parameter + "_snr_comparison.svg"
pio.write_image(fig_snr, save_path_snr_svg, format='svg')

#interactiv eplot save
save_path_snr = r"\\DESKTOP-HLBVG4T\geek\NINETT\Study\Glasgow University\5th Year\DST5\Project files\AIHearingAid\deepNeuronalFilter_optimised\audio_filter\filter_param_comparisons\\" + selected_parameter + "_SNR_comparison.html"
fig_snr.write_html(save_path_snr)

#show
fig_snr.show()

#real-time axes
time, amplitude = plot_time_domain(data, sample_rate)
time2, amplitude2 = plot_time_domain(data2, sample_rate2)
time3, amplitude3 = plot_time_domain(data3, sample_rate3)
time4, amplitude4 = plot_time_domain(data4, sample_rate4)
time5, amplitude5 = plot_time_domain(data5, sample_rate5)

#plot traces for real time plot
trace_rt1 = go.Scatter(x=time, y=amplitude, mode='lines', opacity = op_val, name=dataname1, visible=True)
trace_rt2 = go.Scatter(x=time2, y=amplitude2, mode='lines',opacity = op_val, name=dataname2, visible=True)
trace_rt3 = go.Scatter(x=time3, y=amplitude3, mode='lines',opacity = op_val, name=dataname3, visible=True)
trace_rt4 = go.Scatter(x=time4, y=amplitude4, mode='lines',opacity = op_val, name=dataname4, visible=True)
trace_rt5 = go.Scatter(x=time5, y=amplitude5, mode='lines',opacity = op_val, name=dataname5, visible=True)

#traces
data_rt = [trace_rt1, trace_rt2, trace_rt3, trace_rt4, trace_rt5]

#plotly layout real time plto
layout_rt = go.Layout(
    title= plotname + " by Real-Time Plotting" ,
    title_x=0.5,
    xaxis=dict(title='Time [s]'),
    yaxis=dict(title='Amplitude'),
    showlegend=True
)

#create fig
fig_rt = go.Figure(data=data_rt, layout=layout_rt)

#save svg
save_path_rt_svg = r"\\DESKTOP-HLBVG4T\geek\NINETT\Study\Glasgow University\5th Year\DST5\Project files\AIHearingAid\deepNeuronalFilter_optimised\audio_filter\filter_param_comparisons\\" + selected_parameter + "_rt_comparison.svg"
pio.write_image(fig_rt, save_path_rt_svg, format='svg')

#save html
save_path_rt = r"\\DESKTOP-HLBVG4T\geek\NINETT\Study\Glasgow University\5th Year\DST5\Project files\AIHearingAid\deepNeuronalFilter_optimised\audio_filter\filter_param_comparisons\\" + selected_parameter + "_real_time_comparison.html"
fig_rt.write_html(save_path_rt)

#show
fig_rt.show()
