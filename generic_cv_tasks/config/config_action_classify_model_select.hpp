#pragma once

enum class action_classify_model_enum
{
    pptsm_v2
};

struct config_action_classify_model_select
{
    float confidence_ = 0.25f;
    action_classify_model_enum model_ = action_classify_model_enum::pptsm_v2;
    int sampling_rate_ = 25;
    int top_k_ = 1;    
};
