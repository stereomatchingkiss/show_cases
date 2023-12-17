#pragma once

enum class action_classify_model_enum
{
    pptsm_v2
};

struct config_action_classify_model_select
{
    float confidence_ = 0.25f;
    int top_k_ = 1;
    action_classify_model_enum model_ = action_classify_model_enum::pptsm_v2;
};
