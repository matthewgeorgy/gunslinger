/*================================================================
    * Copyright: 2020 John Jackson
    * simple_triangle

    The purpose of this example is to demonstrate how to explicitly construct 
    GPU resources to use for your application to render a basic triangle.

    Included: 
        * Construct vertex/index buffers from user defined declarations
        * Construct shaders from source
        * Construct pipelines
        * Rendering via command buffers

    Press `esc` to exit the application.
================================================================*/

#define GS_IMPL
#define GS_GRAPHICS_IMPL_CUSTOM
#include "gs_dx11_impl.h"
#include "gs/gs.h"

// All necessary graphics data for this example (shader source/vertex data)
gs_handle(gs_graphics_vertex_buffer_t)   vbo     = {0};
gs_graphics_t *graphics;
gsdx11_data_t *dx11;

void init()
{
	HRESULT 				hr;
	graphics = gs_engine_subsystem(graphics);
	dx11 = (gsdx11_data_t *)graphics->user_data;

	///////////////////////////////////////////////////////////////////////////
	// Shader Setup

	ID3D11VertexShader 		*vs;
	ID3D11PixelShader		*ps;
	ID3DBlob				*vsblob,
							*psblob;

	hr = D3DCompileFromFile(L"vertex.hlsl", NULL, NULL, "main", "vs_5_0", NULL, NULL, &vsblob, NULL);
	hr = D3DCompileFromFile(L"pixel.hlsl", NULL, NULL, "main", "ps_5_0", NULL, NULL, &psblob, NULL);
	hr = ID3D11Device_CreateVertexShader(dx11->device, ID3D10Blob_GetBufferPointer(vsblob),
			ID3D10Blob_GetBufferSize(vsblob), NULL, &vs);
	hr = ID3D11Device_CreatePixelShader(dx11->device, ID3D10Blob_GetBufferPointer(psblob),
			ID3D10Blob_GetBufferSize(psblob), NULL, &ps);
	ID3D11DeviceContext_VSSetShader(dx11->context, vs, 0, 0);
	ID3D11DeviceContext_PSSetShader(dx11->context, ps, 0, 0);

	///////////////////////////////////////////////////////////////////////////
	// Buffer Setup

	UINT	 stride = 3 * sizeof(float),
			 offset = 0;
	gs_graphics_vertex_buffer_desc_t desc = {0};
	float v_data[] = 
	{
		0.0f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f
	};

	desc.data = v_data;
	desc.size = sizeof(v_data);
	vbo = gs_graphics_vertex_buffer_create(&desc);

	ID3D11DeviceContext_IASetVertexBuffers(dx11->context, 0, 1, (gs_slot_array_getp(dx11->vertex_buffers, 1)), &stride, &offset);

	///////////////////////////////////////////////////////////////////////////
	// Input Layout Setup

	ID3D11InputLayout				*Layout;
	D3D11_INPUT_ELEMENT_DESC		LayoutDesc;
	D3D11_VIEWPORT					Viewport;

	LayoutDesc.SemanticName = "POSITION";
	LayoutDesc.SemanticIndex = 0;
	LayoutDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	LayoutDesc.InputSlot = 0;
	LayoutDesc.AlignedByteOffset = 0;
	LayoutDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc.InstanceDataStepRate = 0;

	hr = ID3D11Device_CreateInputLayout(dx11->device, &LayoutDesc, 1, ID3D10Blob_GetBufferPointer(vsblob), ID3D10Blob_GetBufferSize(vsblob), &Layout);
	ID3D11DeviceContext_IASetInputLayout(dx11->context, Layout);
	ID3D11DeviceContext_IASetPrimitiveTopology(dx11->context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = 800;
	Viewport.Height = 600;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	ID3D11DeviceContext_RSSetViewports(dx11->context, 1, &Viewport);

 	///////////////////////////////////////////////////////////////////////////
    // Rasterizer Setup

    ID3D11RasterizerState       *RasterState;
    D3D11_RASTERIZER_DESC       RasterStateDesc;

    RasterStateDesc.FillMode = D3D11_FILL_SOLID;
    RasterStateDesc.CullMode = D3D11_CULL_NONE;
    RasterStateDesc.FrontCounterClockwise = FALSE;
    RasterStateDesc.DepthBias = 0;
    RasterStateDesc.DepthBiasClamp = 0.0f;
    RasterStateDesc.SlopeScaledDepthBias = 0.0f;
    RasterStateDesc.DepthClipEnable = FALSE;
    RasterStateDesc.ScissorEnable = FALSE;
    RasterStateDesc.MultisampleEnable = FALSE;
    RasterStateDesc.AntialiasedLineEnable = FALSE;

	hr = ID3D11Device_CreateRasterizerState(dx11->device, &RasterStateDesc, &RasterState);
    ID3D11DeviceContext_RSSetState(dx11->context, RasterState);
}

void update()
{
    if (gs_platform_key_pressed(GS_KEYCODE_ESC))
		gs_engine_quit();

	// RENDER
	float bgcolor[] = {0.1, 0.1, 0.1, 1.0};

	ID3D11DeviceContext_ClearRenderTargetView(dx11->context, dx11->rtv, bgcolor);
	ID3D11DeviceContext_ClearDepthStencilView(dx11->context, dx11->dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11DeviceContext_Draw(dx11->context, 3, 0);
	IDXGISwapChain_Present(dx11->swapchain, 0, 0);
}

gs_app_desc_t gs_main(int32_t argc, char** argv)
{
    return (gs_app_desc_t)
	{
		.window_width = 800,
		.window_height = 600,
        .init = init,
		.update = update
    };
}   

