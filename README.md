# Torch Content Area
A PyTorch tool kit for segmenting the endoscopic content area in laparoscopy footage. This implementation is based off of that found in ["Detection of circular content area in endoscopic videos"](http://www-itec.uni-klu.ac.at/bib/files/CircleDetection.pdf)

[![Build Status](https://github.com/charliebudd/torch-content-area/actions/workflows/build.yml/badge.svg)](https://github.com/charliebudd/torch-content-area/actions/workflows/build.yml)


## Installation
First ensure you have PyTorch installed and the CUDA toolkit is present on your machine, then run...
```
pip install git+https://github.com/charliebudd/torch-content-area.git@main
```
To avoid runtime errors and achieve optimal performance, ensure that your CUDA toolkit version matches the version of CUDA used by your PyTorch version. Currently PyTorch uses either CUDA 10.2 or CUDA 11.3. 

## Usage

First, import the `ContentAreaInference` class and create an instance...
```
from torchcontentarea import ContentAreaInference

content_area = ContentAreaInference()
```
Then, either infer the content area mask directly...
```
mask = content_area.infer_mask(image)
```
Or return a description of the content area which may be adjusted before drawing the mask...
```
area = content_area.infer_area(image)
area = edit_area(area)
mask = content_area.draw_mask(image, area)
```

## Performance
Performance is tested against the [dataset](testing/data) included in this repo. The follow results were achieved when running on an NVIDIA Quadro RTX 3000...
- Avg Time (Infer Area Only): 0.256ms
- Avg Time (Infer Area and Draw Mask): 0.298ms
- Avg Score (IoU): 0.987
- Misses (IoU < 0.99): 9.7%
- Bad Misses (IoU < 0.95): 4.3%
