#include "infer_area_cpu.h"

torch::Tensor InferAreaHandcrafted::cpu_implementation(torch::Tensor image, uint strip_count, FeatureThresholds feature_thresholds, ConfidenceThresholds confidence_thresholds)
{
    check_image_tensor(image);

    uint batch_count = image.size(0);
    uint image_channels = image.size(1);
    uint image_height = image.size(2);
    uint image_width = image.size(3);
    uint point_count = 2 * strip_count;

    torch::Tensor result = torch::empty({batch_count, 4}, torch::device(image.device()).dtype(torch::kFloat32));

    void* temp_buffer = malloc(3 * batch_count * point_count * sizeof(uint));
    uint*  points_x = (uint*) temp_buffer + 0 * batch_count * point_count;
    uint*  points_y = (uint*) temp_buffer + 1 * batch_count * point_count;
    float* points_s = (float*)temp_buffer + 2 * batch_count * point_count;

    find_points_cpu(image.data_ptr<uint8>(), image_height, image_width, strip_count, feature_thresholds, points_x, points_y, points_s);
    fit_circle_cpu(points_x, points_y, points_s, point_count, confidence_thresholds, image_height, image_width, result.data_ptr<float>());

    free(temp_buffer);

    return result;
}

torch::Tensor InferAreaLearned::cpu_implementation(torch::Tensor image, uint strip_count, torch::jit::Module model, uint model_patch_size, ConfidenceThresholds confidence_thresholds)
{
    check_image_tensor(image);

    uint batch_count = image.size(0);
    uint image_channels = image.size(1);
    uint image_height = image.size(2);
    uint image_width = image.size(3);
    uint point_count = 2 * strip_count;

    torch::Tensor result = torch::empty({batch_count, 4}, torch::device(image.device()).dtype(torch::kFloat32));
    torch::Tensor strips = torch::empty({batch_count * strip_count, 5, model_patch_size, image_width}, torch::device(image.device()).dtype(torch::kFloat32));
    std::vector<torch::jit::IValue> model_input = {strips};

    void* temp_buffer = malloc(3 * batch_count * point_count * sizeof(uint));
    uint*  points_x = (uint*) temp_buffer + 0 * batch_count * point_count;
    uint*  points_y = (uint*) temp_buffer + 1 * batch_count * point_count;
    float* points_s = (float*)temp_buffer + 2 * batch_count * point_count;
    
    make_strips_cpu(image.data_ptr<uint8>(), image_height, image_width, strip_count, model_patch_size, strips.data_ptr<float>());
    
    torch::Tensor strip_scores = torch::sigmoid(model.forward(model_input).toTensor());

    find_points_from_strip_scores_cpu(strip_scores.data_ptr<float>(), image_height, image_width, strip_count, model_patch_size, points_x, points_y, points_s);
    
    fit_circle_cpu(points_x, points_y, points_s, point_count, confidence_thresholds, image_height, image_width, result.data_ptr<float>());

    free(temp_buffer);

    return result;
}
