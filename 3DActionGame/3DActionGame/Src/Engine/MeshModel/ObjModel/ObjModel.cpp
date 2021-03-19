
#include "../../../DirectXTex/DirectXTex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <codecvt> 
#include <iomanip>
#include <cstdio>
#include <sstream>
#include "../../../Utility/Utility.h"
#include "ObjModel.h"


#if _DEBUG
#pragma comment(lib, "Src/DirectXTex/x64/Debug/DirectXTex.lib")
#else
#pragma comment(lib, "Src/DirectXTex/x64/Release/DirectXTex.lib")
#endif


bool ObjModel::Load(const char* fileName_, ID3D11Device* device_, VertexShader* vertex_shader)
{
    
    FILE* fp = nullptr;
    fopen_s(&fp, fileName_, "r");

    // �ǂݍ��݂��������Ă��邩�𒲂ׂ�
    if (fp == nullptr) { return false; }

    std::vector<DirectX::XMFLOAT3> vertices;
    std::vector<DirectX::XMFLOAT3> textures;
    std::vector<DirectX::XMFLOAT3> normals;
    std::string current_mat_name;
    std::vector<std::string> material_list;

    const int line_buffer_length = 1024;
    char line_buffer[line_buffer_length];

    // 1�s�擾����
    while (fgets(line_buffer, line_buffer_length, fp) != nullptr)
    {
        // �R�����g�Ȃ疳������
        if (line_buffer[0] == '#') continue;

        // 1�s�̒�����A�X�y�[�X��T�����̃|�C���^���擾����
        char* parse_point = strchr(line_buffer, ' ');
        // �����X�y�[�X���Ȃ���Ζ�������
        if (parse_point == nullptr) continue;

        Replace('\n', '\0', line_buffer);


        /*
            v  : ���_���W�f�[�^
            vn : �@���f�[�^
            f  : �ʃf�[�^
        */

        // �擪�𒲂ׁuv�v�Ȃ璸�_���Ȃ̂ŉ�͂��s��
        if (line_buffer[0] == 'v')
        {
            // �񕶎��ڂɃA�N�Z�X���Ē��_�̃f�[�^�̎�ނ𒲂ׂ�
            // ���_���W
            if (line_buffer[1] == ' ') {

                ParseVertex(vertices, &parse_point[1]);

                // X���𔽓]�����č��W�n���E��n���獶��n�ɕϊ�
                vertices[vertices.size() - 1].x *= -1.0f;
            }
            // �e�N�X�`�����W
            else if (line_buffer[1] == 't') {
                ParseVertex(textures, &parse_point[1]);
                // V���𔽓]������
                textures[textures.size() - 1].y = (1.0f - textures[textures.size() - 1].y);
            }
            // �@���x�N�g��
            else if (line_buffer[1] == 'n') {

                ParseVertex(normals, &parse_point[1]);

                // X���𔽓]�����č��W�n���E��n���獶��n�ɕϊ�
                normals[normals.size() - 1].x *= -1.0f;
            }
        }
        // �����uf�v�Ȃ�ʏ��Ȃ̂ŉ�͂��s���C���f�b�N�X�o�b�t�@�ɋl�ߍ���
        else if (line_buffer[0] == 'f')
        {
            Replace('\n', '\0', line_buffer);
            ParseFKeywordTag(m_Vertices, current_mat_name, vertices, textures, normals, &parse_point[1]);
        }
        // �����umtllib�v�Ȃ�}�e���A�����Ȃ̂ŕۑ�����
        else if (strstr(line_buffer, "mtllib") == line_buffer)
        {
            Replace('\n', '\0', line_buffer);
            material_list.push_back(&line_buffer[strlen("mtllib") + 1]);
        }
        // �����uusemtl�v�Ȃ珊�����鑮�����ς��̂ŕύX����
        else if (strstr(line_buffer, "usemtl") == line_buffer)
        {
            Replace('\n', '\0', line_buffer);
            current_mat_name = &line_buffer[strlen("usemtl") + 1];
        }
    }

    fclose(fp);

    char flie_path[256];
    int len = strlen(fileName_);
    int path_tail_point = 0;

    for (int i = len - 1; i >= 0; i--)
    {
        if (fileName_[i] == '/')
        {
            path_tail_point = i;
            break;
        }
    }

    strncpy_s(flie_path, fileName_, path_tail_point + 1);
    
    if (LoadMaterialFile(material_list, flie_path, device_) == false) return false;

    if (CreateVertexBuffer(device_) == false) return false;
    if (CreateIndexBuffer(device_) == false)  return false;
    if (CreateInputLayout(device_, vertex_shader) == false) return false;

    return true;
}

void ObjModel::Render(DirectGraphics* graphics_, DirectX::XMFLOAT3 pos_, DirectX::XMFLOAT3 scale_, DirectX::XMFLOAT3 degree)
{
    /*
        ���_�V�F�[�_�̐ݒ�
    */
    ID3D11DeviceContext* context = DirectGraphics::GetInstance()->GetContext();
    context->VSSetShader(DirectGraphics::GetInstance()->GetSimpleVertexShader()->GetShaderInterface(), NULL, 0);
    /*
        �s�N�Z���V�F�[�_�̐ݒ�
    */
    context->PSSetShader(DirectGraphics::GetInstance()->GetSimplePixelShader()->GetShaderInterface(), NULL, 0);

    int  count   = 0;
    UINT strides = sizeof(CVertex);
    UINT offsets = 0;

    for (auto& index : m_Indices)
    {
        graphics_->GetContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &strides, &offsets);
        graphics_->GetContext()->IASetIndexBuffer(m_IndexBuffers[count], DXGI_FORMAT_R32_UINT, 0);

        graphics_->GetContext()->IASetInputLayout(m_InputLayout);

        DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(pos_.x, pos_.y, pos_.z);
        DirectX::XMMATRIX rotate_x = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(degree.x));
        DirectX::XMMATRIX rotate_y = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(degree.y));;
        DirectX::XMMATRIX rotate_z = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(degree.z));;
        DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
        DirectX::XMMATRIX world_matrix = scale * rotate_x * rotate_y * rotate_z * translate;

        // ���[���h�s����R���X�^���g�o�b�t�@�ɐݒ�
        DirectX::XMStoreFloat4x4(&graphics_->GetSimpleConstantBufferData()->World, DirectX::XMMatrixTranspose(world_matrix));

        // �}�e���A���ݒ�
        graphics_->SetMaterial(&m_Materials[index.first]);

        // �R���X�^���g�o�b�t�@�̍X�V
        graphics_->GetContext()->UpdateSubresource(graphics_->GetSimpleConstantBuffer(), 0, nullptr, graphics_->GetSimpleConstantBufferData(), 0, 0);

        // �R���X�^���g�o�b�t�@��ݒ�
        ID3D11Buffer* constant_buffer = graphics_->GetSimpleConstantBuffer();
        graphics_->GetContext()->VSSetConstantBuffers(0, 1, &constant_buffer);
        graphics_->GetContext()->PSSetConstantBuffers(0, 1, &constant_buffer);

        if (m_Textures.count(m_Materials[index.first].TextureKeyWord) > 0)
        {
            graphics_->SetTexture(m_Textures[m_Materials[index.first].TextureKeyWord]);
        }
        else
        {
            graphics_->SetTexture(nullptr);
        }

        // �`��
        graphics_->GetContext()->DrawIndexed(index.second.size(), 0, 0);

        count++;
    }

}

void ObjModel::ParseVertex(std::vector<DirectX::XMFLOAT3>& data_, char* buff_)
{
    std::vector<std::string> split_strings;
    split_strings = Split(buff_, ' ');
    
    int count = 0;
    float value[3] = { 0.f };

    for (std::string str : split_strings)
    {
        value[count] = static_cast<float>(atof(str.c_str()));
        count++;
    }

    data_.push_back(DirectX::XMFLOAT3(value[0], value[1], value[2]));
}

void ObjModel::ParseFKeywordTag(std::vector<CVertex>& outCustomVertices_, std::string current_material_name_, std::vector<DirectX::XMFLOAT3>& vertices_, std::vector<DirectX::XMFLOAT3>& textures_,  std::vector<DirectX::XMFLOAT3>& normals_, char* buffer_)
{
    const int info_num = 3;
    int vertex_info[info_num] =
    {
        -1,// ���_���W
        -1,// �e�N�X�`�����W
        -1 // �@���x�N�g��
    };

    // �ʏ����X�y�[�X��؂�ŕ�����
    std::vector<std::string> space_split = Split(buffer_, ' ');

    for (size_t i = 0; i < space_split.size(); ++i)
    {
        CVertex vertex;
        // �u/�v���ɕ�����
        ParseSlashKeywordTag(vertex_info, (char*)space_split[i].c_str());

        for (int j = 0; j < info_num; ++j)
        {
            if (vertex_info[j] == -1) continue;
            
            int id = vertex_info[j];

            switch (j)
            {
                // ���_���W
            case 0:
                vertex.Pos = vertices_[id];
                break;

                // �e�N�X�`�����W
            case 1:
                vertex.TexturePos = DirectX::XMFLOAT2(textures_[id].x, textures_[id].y);
                break;

                // �@���x�N�g��
            case 2:
                vertex.Normal = normals_[id];
                
                break;
            default:
                break;
            }
        }

        // ���_���X�g�ɒǉ�����
        outCustomVertices_.push_back(vertex);

        // �C���f�b�N�X�o�b�t�@�ɒǉ�
        m_Indices[current_material_name_].push_back(outCustomVertices_.size() - 1);
    }


    int size = m_Indices[current_material_name_].size();

    if (space_split.size() > 3)
    {
        int temp = m_Indices[current_material_name_][size - 1];
        m_Indices[current_material_name_][size - 1] = m_Indices[current_material_name_][size - 4];
        m_Indices[current_material_name_][size - 4] = temp;

        temp = m_Indices[current_material_name_][size - 2];
        m_Indices[current_material_name_][size - 2] = m_Indices[current_material_name_][size - 3];
        m_Indices[current_material_name_][size - 3] = temp;

        m_Indices[current_material_name_].push_back(m_Indices[current_material_name_][size - 4]);
        m_Indices[current_material_name_].push_back(m_Indices[current_material_name_][size - 2]);

        return;
    }
    
     //�|���S���̍쐬�̒��_���Ԃ𔽓]����
    int temp = m_Indices[current_material_name_][size - 1];
    m_Indices[current_material_name_][size - 1] = m_Indices[current_material_name_][size - 3];
    m_Indices[current_material_name_][size - 3] = temp;

}

void ObjModel::ParseSlashKeywordTag(int* list_, char* buffer_)
{
    int counter = 0;

    // �u/�v�ŕ�����𕪉�����
    std::vector<std::string> slash_split = Split(buffer_, '/');

    for (std::string str : slash_split)
    {
        if (str.size() > 0)
        {
            list_[counter] = atoi(str.c_str()) - 1;
        }
        counter++;
    }

}

bool ObjModel::LoadMaterialFile(std::vector<std::string> fileList_, std::string filePath_, ID3D11Device* device_)
{
    const int line_buffer_length = 1024;
    char line_buffer[line_buffer_length];

    for (auto mat_file_name : fileList_)
    {
        FILE* fp = nullptr;
        std::string name = filePath_;
        name += mat_file_name;

        fopen_s(&fp, name.c_str(), "r");

        if (fp == nullptr) return false;

        std::string current_material_name = "";

        while (fgets(line_buffer, 1024, fp) != nullptr)
        {
            // �}�e���A����
            if (strstr(line_buffer, "newmtl") == line_buffer)
            {
                Replace('\n', '\0', line_buffer);
                current_material_name = &line_buffer[strlen("newmtl") + 1];
            }
            // �A���r�G���g
            else if (strstr(line_buffer, "Ka") == line_buffer)
            {
                Replace('\n', '\0', line_buffer);
                std::vector<std::string> split = Split(&line_buffer[strlen("Ka") + 1], ' ');

                for (size_t i = 0; i < split.size(); ++i) {
                    m_Materials[current_material_name].Ambient[i] = atof(split[i].c_str());
                }
            }
            // �f�B�t���[�Y
            else if (strstr(line_buffer, "Kd") == line_buffer)
            {
                Replace('\n', '\0', line_buffer);
                std::vector<std::string> split = Split(&line_buffer[strlen("Kd") + 1], ' ');

                for (size_t i = 0; i < split.size(); ++i) {
                    m_Materials[current_material_name].Diffuse[i] = atof(split[i].c_str());
                }
            }
            // �X�y�L�����[
            else if (strstr(line_buffer, "Ks") == line_buffer)
            {
                Replace('\n', '\0', line_buffer);
                std::vector<std::string> split = Split(&line_buffer[strlen("Ks") + 1], ' ');

                for (size_t i = 0; i < split.size(); ++i) {
                    m_Materials[current_material_name].Specular[i] = atof(split[i].c_str());
                }
            }
            // �e�N�X�`��
            else if (strstr(line_buffer, "map_Kd") == line_buffer)
            {
                Replace('\n', '\0', line_buffer);
                std::string texture_name = &line_buffer[strlen("map_Kd") + 1];

                // �e�N�X�`���܂ł̃p�X��ۑ�
                m_Materials[current_material_name].TextureName = filePath_ + texture_name;
                
                std::vector<std::string> split = Split((char*)texture_name.c_str(), '.');
                if (split.size() > 1)
                {
                    m_Materials[current_material_name].TextureKeyWord = split[0];
                    LoadTexture(split[0], m_Materials[current_material_name].TextureName, device_);
                }

            }

        }
        
        fclose(fp);
    }

    return true;
}

bool ObjModel::LoadTexture(std::string keyWord_, std::string fileName_, ID3D11Device* device_)
{

    if (m_Textures.count(keyWord_) > 0 &&
        m_Textures[keyWord_] != nullptr)
    {
        return true;
    }

    ID3D11ShaderResourceView* shader_res_view;

    const char* extension = "";
    for (size_t i = strlen(fileName_.c_str()); i != 0; --i)
    {
        if (fileName_[i - 1] == '.') extension = &fileName_[i];
    }

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
    std::wstring wstr_file_name = cv.from_bytes(fileName_);

    DirectX::TexMetadata  metadata;
    DirectX::ScratchImage image;
    HRESULT hr;
    
    // DDS�t�@�C���̓ǂݍ���
    if (strcmp(extension, "dds") == 0)
    {
        hr = DirectX::LoadFromDDSFile(wstr_file_name.c_str(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &metadata, image);
    }
    // TGA�t�@�C���̓ǂݍ���
    else if (strcmp(extension, "tga") == 0)
    {
        hr = DirectX::LoadFromTGAFile(wstr_file_name.c_str(), &metadata, image);
    }
    // WIC�t�@�C��(bmp,jpg,png)�̓ǂݍ���
    else
    {
        hr = DirectX::LoadFromWICFile(wstr_file_name.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image);
    }
    
    if (FAILED(hr)) return false;

    hr = DirectX::CreateShaderResourceView(
        device_,
        image.GetImages(),
        image.GetImageCount(),
        metadata,
        &shader_res_view);

    if (FAILED(DirectX::CreateShaderResourceView(
        device_,
        image.GetImages(),
        image.GetImageCount(),
        metadata,
        &shader_res_view)))
    {
        return false;
    }
    
    
    m_Textures[keyWord_] = shader_res_view;

    return true; 
}

bool ObjModel::CreateVertexBuffer(ID3D11Device* device_)
{
    // ���_�o�b�t�@�̍쐬
    D3D11_BUFFER_DESC buffer_desc;
    // �o�b�t�@�̑傫��
    buffer_desc.ByteWidth = sizeof(CVertex) * m_Vertices.size();
    // �o�b�t�@�ւ̊e���ڂł̃A�N�Z�X�����w��
    // ��{�I��D3D11_USAGE_DEFAULT
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    // �o�C���h�̕��@
    // ���_�o�b�t�@��R���X�^���g�o�b�t�@�Ƃ��Ďg�p���邱�Ƃ����߂�
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    // ���\�[�X�ɑ΂��Ă�CPU�A�N�Z�X���@
    // 0���w�肷���CPU�A�N�Z�X���s�v�Ƃ����ݒ�
    buffer_desc.CPUAccessFlags = 0;
    // ���̑��̃I�v�V����
    // �s�v�Ȃ�0
    buffer_desc.MiscFlags = 0;
    // �\���̂̃T�C�Y
    // �o�b�t�@���\���̂Ƃ��Ċm�ۂ���ꍇ�Ɏg�p
    // 0���w�肵�Ă�OK
    buffer_desc.StructureByteStride = 0;

    /*
        ���\�[�X�̏���ݒ肷��\����
        ��L��D3D11_BUFFER_DESC�ƃZ�b�g�ɂȂ��Ă��āA
        �쐬�����o�b�t�@�̏������f�[�^�̎w����@�Ƃ��Ďg����
    */
    D3D11_SUBRESOURCE_DATA sub_resource_data;
    // �o�b�t�@������������f�[�^���w��
    // ���_�o�b�t�@�Ȃ璸�_�\���̂̔z��
    // �C���f�b�N�X�o�b�t�@�Ȃ�C���f�b�N�X��ۑ������f�[�^�̔z��
    // �ȂǂȂ�
    sub_resource_data.pSysMem = &m_Vertices[0];
    // �������̃s�b�`
    // 2D�܂���3D�e�N�X�`���̏ꍇ�Ɏg�p����
    sub_resource_data.SysMemPitch = 0;
    // �[�x���x��
    // 2D�܂���3D�e�N�X�`���̏ꍇ�Ɏg�p����
    sub_resource_data.SysMemSlicePitch = 0;

    /*
    * D3D11_BUFFER_DESC��D3D11_SUBRESOURCE_DATA�̏������ƂɃo�b�t�@���쐬����
    */
    if (FAILED(device_->CreateBuffer(&buffer_desc, &sub_resource_data, &m_VertexBuffer)))
    {
        return false;
    }

    return true;
}

bool ObjModel::CreateIndexBuffer(ID3D11Device* device_)
{
    int count = 0;
    for (auto index : m_Indices)
    {
        m_IndexBuffers.emplace_back();

        D3D11_BUFFER_DESC index_buffer_desc;
        index_buffer_desc.ByteWidth = sizeof(UINT) * index.second.size();
        index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        index_buffer_desc.CPUAccessFlags = 0;
        index_buffer_desc.MiscFlags = 0;
        index_buffer_desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA index_init_data;
        index_init_data.pSysMem = &index.second[0];
        index_init_data.SysMemPitch = 0;
        index_init_data.SysMemSlicePitch = 0;


        if (FAILED(device_->CreateBuffer(&index_buffer_desc, &index_init_data, &m_IndexBuffers[count])))
        {
            return false;
        }

        count++;
    }

    return true;
}

bool ObjModel::CreateInputLayout(ID3D11Device* device, VertexShader* vertex_shader)
{
    D3D11_INPUT_ELEMENT_DESC vertex_desc[]{
        { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXTURE",    0, DXGI_FORMAT_R32G32_FLOAT,        0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    //���_���C�A�E�g�쐬
    if (FAILED(device->CreateInputLayout(
        vertex_desc,				// ���C�A�E�g�ݒ�
        ARRAYSIZE(vertex_desc),		// �z��T�C�Y
        vertex_shader->GetData(),	// ���C�A�E�g�Ɗ֘A�t���钸�_�V�F�[�_�̃f�[�^
        vertex_shader->GetSize(),	// ���C�A�E�g�Ɗ֘A�t���钸�_�V�F�[�_�̃T�C�Y
        &m_InputLayout)))			// �쐬���ꂽ���_���C�A�E�g�̊i�[��
    {
        return false;
    }

    return true;
}