// VertexShaderに送られてくるデータ構造
struct VS_IN
{
    float4 pos          : POSITION0;
    float4 nor          : NORMAL0;
    float2 texture_pos  : TEXCOORD0;
};

// VertexShaderから出力するデータ構造
struct VS_OUT
{
    float4 pos   : SV_POSITION;
    float4 depth : POSITION0;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 World;             //! ワールド行列
    float4x4 View;              //! ビュー行列
    float4x4 Projection;        //! プロジェクション行列
    float4x4 LightView;         //! ライトのビュー行列
    float4x4 ClipUV;            //! UV変換用行列
    float4   CameraPos;         //! カメラ座標
    float4   Light;             //! ライトの方向
    float4   MaterialAmbient;   //! アンビエント光
    float4   MaterialDiffuse;   //! ディヒューズ光
    float4   MaterialSpecular;  //! スペキュラー光
};
VS_OUT vs_main( VS_IN input )
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(input.pos, World);
    output.pos = mul(output.pos, LightView);
    output.pos = mul(output.pos, Projection);
    output.depth = output.pos;

	return output;
}