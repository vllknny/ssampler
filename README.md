# StemSplitterSampler VST3 Plugin

A VST3 audio plugin that splits audio stems using Demucs and maps them to a sampler component.

## Features

- **Real-time Stem Separation**: Uses Demucs neural networks to separate audio into 4 stems:
  - Drums
  - Bass  
  - Other
  - Vocals

- **Sampler Integration**: Automatically loads separated stems into a multi-timbral sampler
- **MIDI Control**: Play separated stems via MIDI notes (C1-F1 mapped to stems 0-3)
- **Stem Level Controls**: Individual volume controls for each stem
- **Quality Settings**: Multiple Demucs model quality levels
- **Output Modes**: Mix all stems or output individual stems

## Architecture

### Core Components

1. **StemSeparator**: Handles Demucs integration and stem separation
2. **SamplerComponent**: Multi-voice sampler with filter and pitch control
3. **PluginProcessor**: Main audio processor coordinating separation and sampling
4. **PluginEditor**: GUI with stem level controls and parameters

### Audio Flow

```
Input Audio → StemSeparator → 4 Stem Buffers → SamplerComponent → Output Audio
                       ↓                      ↓
                 Demucs Model          MIDI Processing
```

## Building

### Prerequisites

- CMake 3.15+
- C++17 compatible compiler
- JUCE 7.0.8+
- (Optional) PyTorch C++ or ONNX Runtime for full Demucs integration

### Build Steps

1. **Clone the project**:
   ```bash
   git clone <repository-url>
   cd StemSplitterSampler
   ```

2. **Get JUCE**:
   ```bash
   git submodule add https://github.com/juce-framework/JUCE.git
   cd JUCE && git checkout 7.0.8 && cd ..
   ```

3. **Configure with CMake**:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

4. **Build**:
   ```bash
   cmake --build . --config Release
   ```

### Windows

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -A x64
cmake --build build --config Release
```

### macOS

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -G Xcode
cmake --build build --config Release
```

### Linux

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

## Installation

### Windows
Copy the built `.vst3` file to:
```
C:\Program Files\Common Files\VST3\
```

### macOS
Copy the built `.vst3` bundle to:
```
~/Library/Audio/Plug-Ins/VST3/
```

### Linux
Copy the built `.vst3` file to:
```
~/.vst3/
```

## Usage

1. Load the plugin in your DAW as an insert effect
2. Send audio through the plugin (any audio source works)
3. The plugin will automatically separate the audio into stems
4. Play MIDI notes C1-F1 to trigger the separated stems:
   - C1: Drums
   - D1: Bass  
   - E1: Other
   - F1: Vocals

### Controls

- **Drum Level**: Volume for drum stem
- **Bass Level**: Volume for bass stem
- **Other Level**: Volume for other instruments stem
- **Vocal Level**: Volume for vocal stem
- **Quality**: Demucs model quality (0-3, higher = better separation but more CPU)
- **Output Mode**: 0 = Mix all stems, 1 = Individual stem outputs

## Demucs Integration

The plugin includes a C++ interface for Demucs neural networks. For full functionality:

### Option 1: PyTorch C++ (Recommended)
- Install PyTorch C++ library
- Place pre-trained Demucs models in `models/` directory
- Models needed:
  - `demucs_light.th` (fast, lower quality)
  - `demucs.th` (balanced)
  - `htdemucs.th` (high quality)
  - `htdemucs_6s.th` (highest quality, 6 stems)

### Option 2: ONNX Runtime
- Convert PyTorch models to ONNX format
- Use ONNX Runtime for inference
- Better performance and deployment options

### Option 3: Python Bridge (Development)
- Use Python interpreter
- Call Demucs via subprocess
- Slower but easier for prototyping

## Development

### Adding New Features

1. **New Stem Types**: Extend `StemSeparator::StemType` enum
2. **Additional Effects**: Add processing in `SamplerComponent::processBlock`
3. **GUI Controls**: Add parameters to `PluginEditor`
4. **MIDI Mapping**: Modify MIDI-to-stem mapping in `SamplerComponent::noteOn`

### Performance Optimization

- Use larger buffer sizes for better Demucs performance
- Implement voice stealing in sampler for polyphony management
- Add SIMD optimizations for audio processing

## License

This project is released under the MIT License.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## Troubleshooting

### Plugin Not Loading
- Check VST3 installation directory
- Verify plugin signature (Windows/macOS)
- Check DAW's plugin scanner logs

### No Audio Output
- Verify audio input is connected
- Check that separation has completed
- Ensure sampler voices are active via MIDI

### High CPU Usage
- Lower the quality setting
- Increase audio buffer size
- Check for CPU throttling in DAW

### Stem Separation Issues
- Verify Demucs models are loaded
- Check model file paths and permissions
- Monitor console for error messages