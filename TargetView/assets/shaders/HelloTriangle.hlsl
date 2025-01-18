#pragma pack_matrix(column_major)

#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
RootConstants(num32BitConstants=16, b0), \
StaticSampler( s2,\
                filter = FILTER_ANISOTROPIC,\
                addressU = TEXTURE_ADDRESS_WRAP,\
                addressV = TEXTURE_ADDRESS_WRAP,\
                addressW = TEXTURE_ADDRESS_WRAP,\
                mipLODBias = 0.f,\
                maxAnisotropy = 16,\
                comparisonFunc = COMPARISON_LESS_EQUAL,\
                borderColor = STATIC_BORDER_COLOR_OPAQUE_WHITE,\
                minLOD = 0.f,\
                maxLOD = 3.402823466e+38f,\
                space = 0,\
                visibility = SHADER_VISIBILITY_ALL), \

struct SmallMVP
{
    float4x4 M;
};

#ifdef VK_HLSL

[[vk::push_constant]] SmallMVP m_SmallMVP;

#else

cbuffer u_SmallMVP : register(b0)
{
    SmallMVP m_SmallMVP;
};

#endif

struct VSInput
{
    [[vk::location(0)]]float3 pos : POSITION;
    [[vk::location(1)]]float4 col : COLOR;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

PSInput vs_main(VSInput vsInput)
{
    PSInput vsoutput;
    vsoutput.pos = mul(float4(vsInput.pos, 1.0f), m_SmallMVP.M);
    vsoutput.col = vsInput.col;
    return vsoutput;
}

float4 ps_main(PSInput psInput) : SV_TARGET0
{
    return psInput.col;
}
