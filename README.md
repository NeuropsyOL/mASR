# Introduction

Electroencephalography (EEG) is a non-invasive method for the recording
of brain-electrical activity. EEG signals can be recorded from several
scalp sites concurrently with small, portable devices. Therefore it is a
strong contender for the interface part of
BCIs.
BCIs are systems that allow a direct link between a human and a computer
by interpreting brain signals in near real-time and translating these
signals to commands for the machine. There is demand for BCIs in the
future to be mobile in order to reach their target audience and be
useful in everyday situations outside of research contexts. As BCIs have to react to changes in
the user's brain state near instantaneously, EEG signal processing for
BCIs has to be capable of low-latency, real-time operation, ideally on
artifact-free data to avoid the influence of artifacts on the system.\
Another field where mobile artifact handling is needed is
neuropsychology research using recordings of human behavior during
motion or natural environments rather than in the laboratory. This implicates
a need for mobile research platforms capable of recording and processing
EEG signals, including before applying specific analyses steps.\
However, both BCI and mobile EEG signals are prone to be contaminated by
artifacts, in movement even more than stationary data.
Artifact correction therefore is indispensable for mobile EEG signal
processing due to the fact that many abnormal data occur irregularly and
with high amplitudes and can thus hinder correct interpretation of the
data.
Artifacts can have a large influence on the performance of signal
processing methods, they may especially break the assumption of some
methods that certain statistical properties remain stable in the data.
Artifact correction for BCIs of the future and for mobile EEG
experiments needs to be capable of online operation on mobile hardware
and needs be able to handle artifacts that occur in mobile EEG.\
One candidate method is artifact subspace reconstruction (ASR), 
a statistical method for artifact reduction in EEG. It
has been shown to perform well with movement artifacts and eye blinks,
artifact classes which are especially important for mobile EEG.\

# The artifact subspace reconstruction algorithm

ASR is a statistical artifact correction method. It detects artifacts
based on their abnormal statistical properties when compared to
artifact-free data. After detection, a correction is applied and the
result of the method are data with the same amount of samples and
channels as the (possibly corrupted) input data. To achieve this
correction, ASR defines a statistical subspace based on presumably
artifact-free calibration data in an initial calibration phase. The
statistical subspace is defined by the distribution of the calibration
data. It is computed based on channel covariance matrices of calibration
data, exploiting the fact that EEG channels correlate highly with their
direct neighbourhood. After
calibration, the subsequent processing phase consists of a comparison of
incoming data chunks to the properties of these artifact-free
calibration data using a similarity measure based on distance in the
statistical subspace. If a given chunk of data is detected to contain
artifactual influences, a correction is applied to the affected segment.
The correction is applied in the statistical subspace as well and is
performed using eigenvectors of covariance matrices of the calibration
data.

ASR benefits from channel configurations that cover a large area due to
the way most artifacts are recorded on many channels at the same time.
Nevertheless, the computational complexity of the method is relatively low, even with large channel
numbers. The reasons lie in the representation of the multi-channel data
in covariance matrices which remain of low dimensionality while
preserving relevant statistical properties such as covariance among
neighboring channels.

However, because ASR is implemented in Matlab, in its current
implementation, thereafter called *reference implementation*, ASR cannot
be used in mobile EEG recordings easily. Matlab needs a decently
powerful PC and is only available for operating
systems running on an Intel-architecture processor. Mobile recording
hardware is typically not fast enough to run Matlab and usually sports
some variation of ARM architecture. There have been efforts to modify
ASR for mobile devices, but they aim mainly at field
programmable gate arrays (FPGAs), which are more specialised and harder
to use for the average scientist, and there is no open source
implementation of mobile ASR as of yet.\
Therefore we propose mobile ASR (mASR), an implementation of ASR for
limited hardware, in our case a research platform to evaluate audio/time
domain/hearing aid signal processing. In this report, we describe the
architecture, the process of translating and compiling a Matlab codebase
for this research platform, and a set of validation tests using publicly
available data sets. With the implementation of ASR on portable
hardware, the interpretation of neural data in different contexts is
possible. Furthermore, we hope that this report provides a guideline for
others looking to translate Matlab code into compiled languages for
usage on limited hardware.\

Our target hardware is the Portable Hearing Laboratory (PHL), 
a mobile integrated setup for
audiological research based on the [Beaglebone Black
Wireless]{.smallcaps} single-board computer and the open-source audio interface
[Cape4All]{.smallcaps}. It includes a battery and a set of
binaural behind-the-ear (BTE) hearing aids, and runs a software platform
for various audio signal processing algorithms, openMHA,
on a Linux distribution optimized for low-latency real-time audio
processing. The PHL has been and is being used for mobile audiological
experiments in the past.
There are currently efforts ongoing to integrate EEG sensor data into
the the openMHA audio processing, fueled by the rising need of smarter
hearing aids that adapt the audio processing based on context
information. Its established
user base together with the urgent need for EEG artifact reduction makes
the PHL an attractive target platform for mASR.\

# openMHA in a nutshell

Since openMHA is described in detail [https://www.openmha.org](elsewhere)
we only introduce some concepts especially
important for our plugin. The openMHA main executable acts as a plugin
host while the plugins provide the actual signal processing. Every
plugin needs to implement the `prepare()`, `process()` and `release()`
functions. The `prepare()` method is called before the processing
starts. It locks in the input signal dimensions and provides the
framework with the output signal dimensions. `process()` is the main
processing method. It receives the input signal and returns the output
signal and is called periodically by the framework. The `release()`
method is used to free resources after processing. The openMHA framework
knows different kinds of variables: [Algorithm communication
variables]{.smallcaps} (AC variables) are used to share side-channel
information between plugins while [configuration variables]{.smallcaps}
are used to configure plugins. openMHA provides a mechanism to change
configuration variables during audio processing in a real-time safe
manner.

# Implementation and architecture

In order to reduce development time the Matlab Coder was used to
automatically translate a simplified version of the reference
implementation to C++. In order to be able to use the automatic
translation from Matlab to C or C++ code, some adaptations to the
original implementation needed to be made. They are described later in
*Translation of ASR to Matlab Coder-compatible source base*. Apart from
the simplification, the translation process was automated by using
Makefiles, thereby shortening the time it takes to integrate
improvements in the reference implementation and reducing the potential
for human error.

#### mASR in the openMHA framework {#masr-in-the-openmha-framework .unnumbered}

Our implementation consists of several files, of which the most
important are:

codegen

:   contains the C++ code generated by the Matlab coder, including the
    so-called entry-point functions `asr_calibrate` and `asr_process`,
    which are the equivalent of the Matlab functions of the same name.

Makefile

:   The Makefile providing build automation.

asr_calibrate_simple.m

:   The simplified version of the asr_calibrate.m script.

asr_process_simple.m

:   is the simplified version of asr_process.m

make.m

:   steers the Matlab Coder configuration.

asr.cpp

:   contains the implementation of the openMHA plugin

Our plugin implementation follows the usual structure of openMHA plugins
and implements the usual openMHA callback functions. The incoming EEG
signal is provided via AC variable, the usual inter-plugin communication
method in openMHA. The `stride` attribute of the AC variable represents
the number of channels in the EEG signal, which has the same
requirements as the input signal of the reference implementation. To
import the signal into openMHA, other plugins have to be used, for
example the `lsl2ac`-plugin, which converts LSL streams
into AC variables. The most important functions are the `prepare()`, the
`process()`, and the `release()` functions. The `prepare()` function is
where most initialization takes place and the configuration variables
are locked in. It also performs the calibration. The calibration is
provided as csv file, where every row presents a channel. `process()` is
the main processing function. It is called periodically by the audio
processing thread of the openMHA executable. To ensure real-time safety,
the actual artifact reduction happens in a separate thread. The input
data is shared between the threads safely by means of a lock-free
first-in, first-out (FIFO) buffer. The output of the cleaning algorithm
is then shared with the processing thread via another FIFO buffer and
made accessible to the downstream plugins as an AC variable. The output
AC variable can then processed within openMHA or be exported and
processed elsewhere, e.g. via the `ac2lsl` plugin. `release()` signals
the ASR thread to terminate and unlocks the configuration variables. The
plugin has the following configuration variables:

SamplingRate

:   the nominal sampling rate of the input data in Hz.

WindowLength

:   Length of the statistics window, in seconds (e.g., 0.5). This should
    not be much longer than the time scale over which artifacts persist,
    but the number of sample in the window should not be smaller than
    1.5x the number of channels.

VarName

:   Name of the input AC variable.

CalibrationFileName

:   file name of the calibration file.

#### Translation of ASR to Matlab Coder-compatible source base

In order to be able to generate C/C++ code from the ASR Matlab codebase,
some steps with regards to compatibility to the Matlab code generation
needed to be implemented. Firstly, all unsupported functions were
replaced (complete list of supported functions:
<https://de.mathworks.com/help/referencelist.html>. This included the
unrolling of the `bsxfun` function into explicit loops
(<https://de.mathworks.com/help/matlab/ref/bsxfun.html>). `bsxfun`
allows to apply element-wise operations by providing a function and an
array, `bsxfun` implicitly expands the application to all elements and
the resulting operation can be equivalently expressed by looping through
the array. Another example is slicing of vectors or array sections,
which the Matlab Coder cannot expand to all datatypes automatically.
Instead, an explicit indexing was implemented at the corresponding code
locations. Then, all variables were declared with their desired size and
initialized before their first usage in both `asr_calibrate.m` and
`asr_process.m`. Subsequently tests were performed to ensure the
equivalency of the adapted ASR code to the original code base.

# Quality control

In order to ensure that mASR works correctly, we processed data sets
using the reference implementation and our implementation and then
compared the results of `asr_calibrate_simple()` and its C++-counterpart
and `asr_process_simple()` and its C++-counterpart. We deem the test for
`asr_process_simple()` passed when all outputs of the calibration and
processing have the same value as the output of the reference
implementation, up to a relative tolerance of $10^-5$. Test data adapted
from a publicly available data set are included in the
mASR distribution on GitHub. In order to run the tests, the user needs
to run

    make test; ./test

within the main repository directory.
