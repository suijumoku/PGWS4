// 頂点シェーダー化ピクセルシェーダーへのやり取りに使用する構造体
struct Output
{
    float4 svpos : SV_Position; // システム用頂点座標
    float4 normal : NORMAL; // 法線ベクトル
    float2 uv : TEXCOORD; // uv値
};

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されてサンプラー

cbuffer cbuff0 : register(b0)
{
    matrix world; // ワールド変換行列
    matrix viewproj; // ビュープロジェクション行列
};