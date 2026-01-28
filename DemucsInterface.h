#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// DemuC++ Interface - C wrapper for Demucs functionality
typedef struct DemucsModel DemucsModel;

// Model management
DemucsModel* demucs_load_model(const char* model_path, int sample_rate);
void demucs_cleanup(DemucsModel* model);

// Audio processing
void demucs_separate(DemucsModel* model, 
                    const float* input_stereo,
                    float** outputs, // Array of 4 output buffers (drums, bass, other, vocals)
                    int num_samples);

// Utility functions
int demucs_get_sample_rate(const DemucsModel* model);
int demucs_get_stem_count(const DemucsModel* model);

#ifdef __cplusplus
}
#endif