import matplotlib.pyplot as plt
import math
import numpy as np
import pandas as pd
from scipy.signal import find_peaks, peak_widths
from scipy.signal import butter, filtfilt
from scipy.fft import fft, fftfreq
import os

### Funktioner för att räkna ut spektrala centroiden och bandbredden kring just grundfrekvensen
def _spectral_centroid(spectrum: np.ndarray, samplerate: float):
    ps = np.abs(spectrum) ** 2
    ps_sum = 0.0
    ps_sum_weighted = 0.0
    for i, magnitude in enumerate(ps):
        ps_sum += magnitude
        ps_sum_weighted += magnitude * i
    return 0.5 * samplerate / (len(ps) - 1) * (ps_sum_weighted / ps_sum)


def spectral_variance(spectrum: np.ndarray, samplerate: float):
    f_centroid = _spectral_centroid(spectrum, samplerate)
    ps = np.abs(spectrum) ** 2
    ps_sum = 0.0
    ps_sum_weighted = 0.0
    for i, magnitude in enumerate(ps):
        f = 0.5 * samplerate / (len(ps) - 1) * i
        ps_sum += magnitude
        ps_sum_weighted += magnitude * (f - f_centroid) ** 2
    return ps_sum_weighted / ps_sum

def bandpass_filter(data, lowcut, highcut, sampling_rate):
    nyquist = 0.5 * sampling_rate
    low = lowcut / nyquist
    high = highcut / nyquist
    b, a = butter(4, [low, high], btype='band', analog=False)
    return filtfilt(b, a, data)

################################## skapa tomma lister och loopa igeonom filer i en mapp

lista_namn,lista_samplingrate, lista_fwhm_v, lista_peak_v, lista_bredd02_v, lista_fwhm_h, lista_peak_h, lista_bredd02_h, lista_skillnad_fwhm, lista_skillnad_02bredd, lista_energiandel, lista_energiandel2, lista_bandbredd, lista_bandbredd2= [],[],[],[],[],[],[],[],[],[],[],[],[],[]
folder_path = r"C:\Users\super\OneDrive - Chalmers\Kandidatarbete\Testfiler"

min_length = float('inf')
for filename in os.listdir(folder_path):
    file_path = os.path.join(folder_path, filename)
    if os.path.isfile(file_path):
        print("Fil:", file_path)
        log_df_bra = pd.read_csv(file_path, header=None, delimiter=' ', skiprows=1, on_bad_lines='skip')

        print(log_df_bra.shape)
        print(log_df_bra.head())

        current_len = len(log_df_bra)
        if current_len < min_length:
            min_length = current_len


        log_df_bra = log_df_bra.drop(columns=[ 1,5,9,11,12,13,15,17,19,20,21,23,25])
        log_df_bra.columns = ['Time','L1', 'L2', 'L3', 'R1', 'R2', 'R3', 'D1', '1X', '1Y', '1Z', '2X', '2Y', '2Z']


        # samplingrate
        time_diff_ms = log_df_bra['Time'].iloc[-2] - log_df_bra['Time'].iloc[1] #skippa vissa rader då de ofta är fel och saknar tidsstämpel
        time_diff_s = time_diff_ms/1000
        samplingrate = (len(log_df_bra)-1)/time_diff_s
        print('samplingrate:', samplingrate)
        lista_samplingrate.append(samplingrate)
        lista_namn.append(filename)

        # normalisering 

        for col in ['L1', 'L2', 'L3', 'R1', 'R2', 'R3']:
            log_df_bra[col] = (log_df_bra[col] - log_df_bra[col].min()) / (log_df_bra[col].max() - log_df_bra[col].min())

        log_df_bra.head()

        vanster_fot = log_df_bra[['L1', 'L2','L3']].sum(axis=1)
        hoger_fot = log_df_bra[['R1', 'R2','R3']].sum(axis=1)

        
        block_size = min_length ### Bestäm blocksize
        print(min_length)
        total_length = len(log_df_bra)

        total_peak = 0
        total_fwhm = 0 
        total_02_bredd = 0
        block_counter = 0

        total_peak_2 = 0
        total_fwhm_2 = 0 
        total_02_bredd_hoger = 0
        block_counter_2 = 0
        for start in range(0, total_length, block_size):
            slut = start + block_size
            block_counter = block_counter + 1
            block_counter_2 = block_counter_2 +1
            if slut > total_length:
                break  # avsluta om vi inte har blocksize antal kvar

        #######################################################################################################################
            y = vanster_fot[start:slut].values
            y = y - np.mean(y)
            N = len(y)
            T = 1.0/samplingrate 
            yf = fft(y)

            xf = fftfreq(N, T)
            yf_abs = 2.0 / N * np.abs(yf[:N // 2])
            xf_pos = xf[:N // 2]

            window_size = 20
            yf_abs[0] = 0
            yf_abs_con = np.convolve(yf_abs, np.ones(window_size)/window_size, mode='same')

            signal = vanster_fot[start:slut].values  
            signal = signal - np.mean(signal)  

            # Skapa tidsaxel
            N = len(signal)
            t = np.arange(N) / samplingrate


            window_size = 20  # Antal samples i fönstret (justera efter behov)
            signal_smoothed = np.convolve(signal, np.ones(window_size)/window_size, mode='same')
            
            
            plt.figure(figsize=(10, 4))
            plt.plot(t, signal, label='Originalsignal', alpha=0.5)
            plt.plot(t, signal_smoothed, label='Glidande medelvärde', linewidth=2)
            plt.xlabel("Tid (s)")
            plt.ylabel("Amplitude")
            plt.title("Signal i tidsdomän (med utjämning)")
            plt.grid(True)
            plt.legend()
            plt.tight_layout()
            plt.show()

           

        ################################################ höger fot 
            yh = hoger_fot[start:slut].values
            yh = yh - np.mean(yh)
            Nh = len(yh)
            T = 1.0/samplingrate 
            yfh = fft(yh)
            
            xfh = fftfreq(Nh, T)
            yf_absh = 2.0 / Nh * np.abs(yfh[:Nh // 2])
            xf_posh = xfh[:Nh // 2]
            yf_absh[0] = 0

            window_size = 20
            yf_abs_con_hoger = np.convolve(yf_absh, np.ones(window_size)/window_size, mode='same')

        ####### VÄNSTER FOT
        # ---- Grundfrekvens & FWHM ----
            peaks, _ = find_peaks(yf_abs_con)
            main_peak_idx = np.argmax(yf_abs_con[peaks])
            main_peak = peaks[main_peak_idx]
            results_half = peak_widths(yf_abs_con, [main_peak], rel_height=0.5)
            peak_freq = xf[main_peak]

            results_2 = peak_widths(yf_abs_con, [main_peak], rel_height=0.8)

            lista_peak_v.append(peak_freq)

        # Interpolera frekvenserna vid halvmax-bredden
            x_interp = np.interp(results_half[2:4], np.arange(len(xf_pos)), xf_pos)
            left_freq, right_freq = x_interp
            fwhm_hz = right_freq - left_freq

            x_interp_2 = np.interp(results_2[2:4], np.arange(len(xf_pos)), xf_pos)
            left_freq_2, right_freq_2 = x_interp_2
            bredd_hz_2 = right_freq_2 - left_freq_2

        ######## HÖGER FOT
            peaksh, _hoger = find_peaks(yf_abs_con_hoger)
            main_peak_idx_hoger = np.argmax(yf_abs_con_hoger[peaksh])
            main_peak_hoger = peaksh[main_peak_idx_hoger]
            results_half_hoger = peak_widths(yf_abs_con_hoger, [main_peak_hoger], rel_height=0.5)
            peak_freq_hoger = xf[main_peak_hoger]

            lista_peak_h.append(peak_freq_hoger)

            results_2_hoger = peak_widths(yf_abs_con_hoger, [main_peak_hoger], rel_height=0.8)

        # Interpolera frekvenserna vid halvmax-bredden
            x_interp_hoger = np.interp(results_half_hoger[2:4], np.arange(len(xf_posh)), xf_posh)
            left_freq_hoger, right_freq_hoger = x_interp_hoger
            fwhm_hz_hoger = right_freq_hoger - left_freq_hoger

            x_interp_hoger_2 = np.interp(results_2_hoger[2:4], np.arange(len(xf_posh)), xf_posh)
            left_freq_hoger_2, right_freq_hoger_2 = x_interp_hoger_2
            bredd_hz_hoger = right_freq_hoger_2 - left_freq_hoger_2

        # ---- Plot ---- viktigt att kolla så att FWHM inte ser helt skit ut
            plt.figure(figsize=(10, 5))
            plt.plot(xf_pos, yf_abs_con, label='FFT')
            plt.plot(xf_pos[main_peak], yf_abs_con[main_peak], 'ro', label='Grundfrekvens')
            plt.hlines(results_half[1][0], left_freq, right_freq, colors='C2', label='FWHM', linewidth=2)
            plt.axvline(left_freq, color='C2', linestyle='--')
            plt.axvline(right_freq, color='C2', linestyle='--')
            plt.hlines(results_2[1][0], left_freq_2, right_freq_2, colors='C6', label='Bredd 20% maximala amplituden', linewidth=2)
            plt.axvline(left_freq_2, color='C6', linestyle='--')
            plt.axvline(right_freq_2, color='C6', linestyle='--')
            plt.title("Frekvensspektrum med exakt FWHM – Vänster fot")
            plt.xlabel("Frekvens (Hz)")
            plt.ylabel("Amplitud")
            plt.grid(True)
            plt.legend()
            plt.tight_layout()

            # Figur 2 – Höger fot
            plt.figure(figsize=(10, 5))
            plt.plot(xf_posh, yf_abs_con_hoger, label='FFT')
            plt.plot(xf_posh[main_peak_hoger], yf_abs_con_hoger[main_peak_hoger], 'ro', label='Grundfrekvens')
            plt.hlines(results_half_hoger[1][0], left_freq_hoger, right_freq_hoger, colors='C2', label='FWHM', linewidth=2)
            plt.axvline(left_freq_hoger, color='C2', linestyle='--')
            plt.axvline(right_freq_hoger, color='C2', linestyle='--')
            plt.hlines(results_2_hoger[1][0], left_freq_hoger_2, right_freq_hoger_2, colors='C6', label='Bredd 20% maximala amplituden', linewidth=2)
            plt.axvline(left_freq_hoger_2, color='C6', linestyle='--')
            plt.axvline(right_freq_hoger_2, color='C6', linestyle='--')
            plt.title("Frekvensspektrum med exakt FWHM – Höger fot")
            plt.xlabel("Frekvens (Hz)")
            plt.ylabel("Amplitud")
            plt.grid(True)
            plt.legend()
            plt.tight_layout()

            plt.show()

            print('FWHM: vänster', fwhm_hz)
            print('peak vänster:', peak_freq)
            print('bredd 0.2 vänster:', bredd_hz_2)
            total_fwhm = total_fwhm + fwhm_hz
            total_02_bredd = total_02_bredd + bredd_hz_2
            total_peak = total_peak + peak_freq

            avgerege_FWHM = (total_fwhm)/block_counter
            averege_02bredd = total_02_bredd/block_counter
            averege_peak = total_peak/block_counter

            print('FWHM: höger', fwhm_hz_hoger)
            print('peak höger:', peak_freq_hoger)
            print('bredd 0.2 höger:', bredd_hz_hoger)
            total_fwhm_2 = total_fwhm_2 + fwhm_hz_hoger
            total_02_bredd_hoger = total_02_bredd_hoger + bredd_hz_hoger
            total_peak_2 = total_peak_2 + peak_freq_hoger
            avgerege_FWHM_hoger = (total_fwhm_2)/block_counter_2
            averege_02bredd_hoger = total_02_bredd_hoger/block_counter_2
            averege_peak_hoger = total_peak_2/block_counter_2

            diff_medel_02bredd = abs(averege_02bredd - averege_02bredd_hoger) / ((averege_02bredd + averege_02bredd_hoger) / 2)
            diff_medel_fwhm = abs(avgerege_FWHM - avgerege_FWHM_hoger) / ((avgerege_FWHM + avgerege_FWHM_hoger) / 2)

            lista_bredd02_v.append(averege_02bredd[0])
            lista_bredd02_h.append(averege_02bredd_hoger[0])
            lista_fwhm_v.append(avgerege_FWHM[0])
            lista_fwhm_h.append(avgerege_FWHM_hoger[0])
            lista_skillnad_02bredd.append(diff_medel_02bredd[0])
            lista_skillnad_fwhm.append(diff_medel_fwhm[0])

            #andel energi grundfrekvens med band på 0.05 Hz, någonting är off

            f0 = peak_freq 
            band_width = 0.2
            mask = (xf_pos >= f0 - band_width) & (xf_pos <= f0 + band_width)
            energy_fundamental = np.sum(yf_abs_con[mask]**2)
            total_energy = np.sum(yf_abs_con**2)
            energy_ratio = energy_fundamental / total_energy
            lista_energiandel.append(energy_ratio)

            f02 = peak_freq_hoger 
            band_width = 0.2
            mask2 = (xf_posh >= f02 - band_width) & (xf_posh <= f02 + band_width)
            energy_fundamental2 = np.sum(yf_abs_con_hoger[mask2]**2)
            total_energy2 = np.sum(yf_abs_con_hoger**2)
            energy_ratio2 = energy_fundamental2 / total_energy2
            lista_energiandel2.append(energy_ratio2)

            ############################# spektrala bandbredden
            yf_filter = bandpass_filter(y, lowcut=peak_freq-0.25, highcut=peak_freq+0.25, sampling_rate=samplingrate)
            yf_filtered = fft(yf_filter)
            N = len(yf_filtered)
            yf_half = yf_filtered[:N // 2].copy()
            yf_half[0] = 0
            
            
            yfh_filter = bandpass_filter(yh, lowcut=peak_freq_hoger-0.25, highcut=peak_freq_hoger+0.25, sampling_rate=samplingrate)
            yfh_filtered = fft(yfh_filter)
            N = len(yfh_filtered)
            yfh_half = yfh_filtered[:N // 2].copy()
            yfh_half[0] = 0

            bandbredd = math.sqrt(spectral_variance(yf_half, samplingrate))
            bandbredd2 = math.sqrt(spectral_variance(yfh_half, samplingrate))

            lista_bandbredd.append(bandbredd)
            lista_bandbredd2.append(bandbredd2)
        
        print(f"Andel energi i grundfrekvensbandet: {energy_ratio:.2%}")
        print(f"Andel energi i grundfrekvensbandet: {energy_ratio2:.2%}")

        print('bandbredd' , bandbredd)
        print('bandbredd2' , bandbredd2)

        print('medelvärde i fwhm vänster', avgerege_FWHM)
        print('medelvärde grundfrekvens vänster', averege_peak)
        print('medelvärde i bredd vid 0.2 vänster fot:', averege_02bredd)

        print('medelvärde i fwhm höger', avgerege_FWHM_hoger)
        print('medelvärde grundfrekvens höger', averege_peak_hoger)
        print('medelvärde i bredd vid 0.2 höger fot:', averege_02bredd_hoger)



        print('skillnad 02bredd:', abs(diff_medel_02bredd))
        print('skillnad fwhm höger och vänster:', abs(diff_medel_fwhm))


        df = pd.DataFrame(list( zip (lista_namn, lista_samplingrate, lista_peak_h , lista_peak_v  , lista_bredd02_h ,
        lista_bredd02_v , lista_skillnad_02bredd , lista_fwhm_v , lista_fwhm_h, lista_skillnad_fwhm, lista_energiandel, lista_energiandel2, lista_bandbredd, lista_bandbredd2)), columns =[ 'filnamn' , 'samplingrate' , 'grundfrekvens höger' , 'grundfrekvens vänster' , 'bredd 20% höger' , 'bredd 20% vänster' , 'diff 20% höger/vänster' , 'FWHM vänster' ,'FWHM höger', 'diff FWHM höger/vänster', 'energiandel vänster', 'energiandel höger', 'bandbredd vänster', 'bandbredd höger' ]) # create dataframe

df.to_csv(r"C:\Users\super\OneDrive - Chalmers\Kandidatarbete\dataframe_frekvens6.csv", index=None)


