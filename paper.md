---
title: 'Mobile EEG artifact correction on limited hardware using artifact subspace reconstruction'
tags:
  - mobile EEG
  - artifact-correction
  - data-processing
  - ASR
authors:
  - name: Paul Maanen
    orcid: 0000-0002-7989-5112
    corresponding: true
    affiliation: "1, 2" # (Multiple affiliations must be quoted)
  - name: Sarah Blum
    orcid: 0000-0002-7129-8962
    affiliation: 3
  - name: Stefan Debener
    orcid: 0000-0003-4265-5542
    affiliation: "1,2"
affiliations:
 - name: Department of Psychology, Carl von Ossietzky Universität Oldenburg, 26129 Oldenburg, Germany
   index: 1
 - name: Cluster of Excellence “Hearing4All”, Carl von Ossietzky Universität Oldenburg, Germany
   index: 2
 - name: Hörzentrum Oldenburg gGmbH, Oldenburg, Germany
   index: 3
date: 13 August 2017
bibliography: paper.bib

---

# Summary

Biological data like electroencephalography (EEG) are typically contaminated  by unwanted signals, called artifacts. Therefore, many applications dealing with biological data with low signal-to-noise ratio require robust artifact correction. For some applications like brain-computer-interfaces (BCI), the artifact correction needs to be real-time capable.  Artifact subspace reconstruction (ASR) is a statistical method for artifact reduction in EEG. However, in its current implementation, ASR cannot be used in mobile data recordings using limited hardware easily. In this report, we add to the growing field of portable, online signal processing methods by describing an implementation of ASR for limited hardware like single-board computers. We describe the architecture, the process of translating and compiling a Matlab codebase for a research platform, and a set of validation tests using publicly available data sets. The implementation of ASR on limited, portable hardware facilitates the online interpretation of EEG signals acquired outside of the laboratory environment.

# Statement of need

Electroencephalography (EEG) is a non-invasive method for the recording of brain-electrical activity. EEG signals can be recorded from several scalp sites concurrently with small, portable devices. Therefore it is a strong contender for the interface part of BCIs [@lotte2014, @bottelecocq2014, @blankertz2016, @liyanage2020, @zander2011]. BCIs are systems that allow a direct link between a human and a computer by interpreting brain signals in near real-time and translating these signals to commands for the machine. There is demand for BCIs in the future to be mobile in order to reach their target audience and be useful in everyday situations outside of research contexts @[dauwels2016, na2021, ogino2020]. As BCIs have to react to changes in the user's brain state near instantaneously, EEG signal processing for BCIs has to be capable of low-latency, real-time operation, ideally on artifact-free data to avoid the influence of artifacts on the system.

Another field where mobile artifact handling is needed is neuropsychology research using recordings of human behavior during motion or natural environments rather than in the laboratory [@jacobsen2020, @debener2012, @devos2014, @devos2014b] . This implicates a need for mobile research platforms capable of recording and processing EEG signals, including before applying specific analyses steps.

However, both BCI and mobile EEG signals are prone to be contaminated by artifacts, in movement even more than stationary data @[jacobsen2020]. Artifact correction therefore is indispensable for mobile EEG signal processing due to the fact that many abnormal data occur irregularly and with high amplitudes and can thus hinder correct interpretation of the data [chandola2009, @blum2019, @mane2020, @chaudhary2021, @alchalabi2021]. Artifacts can have a large influence on the performance of signal processing methods, they may especially break the assumption of some methods that certain statistical properties remain stable in the data. Artifact correction for BCIs of the future and for mobile EEG experiments needs to be capable of online operation on mobile hardware and needs be able to handle artifacts that occur in mobile EEG. 

One candidate method is artifact subspace reconstruction (ASR) @mullen2015, a statistical method for artifact reduction in EEG. It has been shown to perform well with movement artifacts and eye blinks [@blum2019, @jacobsen2020, @ladouce2021, @dehais2020, @plechawska2018, @nathan2016, @nordin2018, @dehais2020], artifact classes which are especially important for mobile EEG.

ASR is a statistical artifact correction method [@mullen2015, @blum2019, @chang2018, @pion2018]. It detects artifacts based on their abnormal statistical properties when compared to artifact-free data. After detection, a correction is applied and the result of the method are data with the same amount of samples and channels as the (possibly corrupted) input data.

However, because ASR is implemented in Matlab, in its current implementation, ASR cannot be used in mobile EEG recordings easily. Matlab needs a decently powerful PC @matlab-requirements and is only available for operating systems running on an Intel-architecture processor. Mobile recording hardware is typically not fast enough to run Matlab and usually sports some variation of ARM architecture.
There have been efforts to modify ASR for mobile devices @van2021, but they aim mainly at field programmable gate arrays (FPGAs), which are more specialised and harder to use for the average scientist, and there is no open source implementation of mobile ASR as of yet.

Therefore we propose mobile ASR (mASR), an implementation of ASR for limited hardware, in our case a research platform to evaluate audio/time domain/hearing aid signal processing. In this report, we describe the architecture, the process of translating and compiling a Matlab codebase for this research platform, and a set of validation tests using publicly available data sets. With the implementation of ASR on portable hardware, the interpretation of neural data in different contexts is possible. Furthermore, we hope that this report provides a guideline for others looking to translate Matlab code into compiled languages for usage on limited hardware.

# Acknowledgements

We like to thank the original authors and current maintainers of the Matlab ASR implementation.

# Funding statement

This work was supported by the DFG Cluster of Excellence EXC 1077/1 "Hearing4all".

# References
