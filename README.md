# Audio Limiter

This is an audio limiter, a tool to restrict audio signals to a specific range
(typically -1 to 1) while reducing perceived distortion.

This audio limiter works by filling a delay line with samples of the incoming
signal. This delay line is searched for peaks that exceed the range even after
being multiplied by the attenuation signal. If a peak is found, the attenuation
signal is adjusted to make that peak lie within the range. The attenuation
signal is the convolution of impulses placed some samples before the signal
value to be attenuated and a signal consisting of half of a Hann window for the
ramp up, and an exponential decay of an infinite impulse-response (IIR) filter
for the ramp down. In this way, the attenuation signal can be updated
efficiently for future values (simply by computing the output of the filter).

## Noise Gate

There is also a very simple noise gate. This simply makes the signal 0 by multiplying with an attenuation signal if the power as estimated over a local window is less than some threshold. There is some fading done to smooth the attenuation signal.

## Using

### Limiter

Some optimization has been done to use this on the ARM Cortex-M4 architecture.
See the `liblimiter_ir_af.a` recipe in `arm_cm4.mk` for a command line that will
compile the limiter into a library for this architecture.

You can also compile a simple program that will limit little-endian float-32
encoded files. See the `bin/limit_mono_f32` recipe of `tools/Makefile`

### Noise Gate

To compile a simple program that applies a noise gate to a little-endian
float-32 encoded file, see the `bin/noise_gate_mono_f32` recipe of
tools/Makefile.

## Copyright, License and Warranty

All source code in this repository is Copyright (c) 2019 Nicholas Esterer.

All source code in this repository is covered under the GNU General Public
License V3 (https://www.gnu.org/licenses/gpl-3.0.en.html).

Please contact nicholas dot esterer at gmail dot com for other licensing.

This software comes with absolutely no warranty.
