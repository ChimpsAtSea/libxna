#include "../include/Content.hpp"

namespace XNA {
namespace Content {

//ContentBase* Read(BinaryReader& reader, const std::string& type_reader_name)
std::shared_ptr<ContentBase> ContentBase::Read(BinaryReader& reader, const std::string& type_reader_name)
{
	if(type_reader_name == "Microsoft.Xna.Framework.Content.Texture2DReader")
	{
		//return new Texture2D(reader);
		return std::make_shared<Texture2D>(reader);
	}
	if(type_reader_name == "Microsoft.Xna.Framework.Content.SoundEffectReader")
	{
		//return new Sound(reader);
		return std::make_shared<Sound>(reader);
	}
	throw ("unknown type reader: " + type_reader_name);
}

std::string ContentBase::get_type_reader_name()
{
	return this->type_reader_name;
}

Texture2D::Texture2D(BinaryReader& reader)
{
	this->type_reader_name = "Microsoft.Xna.Framework.Content.Texture2DReader";
	this->read(reader);
}

std::vector<uint8_t> Texture2D::get_mip_data(uint_fast32_t i)
{
	return this->mips[i];
}

std::pair<uint32_t, uint32_t> Texture2D::get_mip_size(uint_fast32_t i)
{
	return std::make_pair(this->width >> i, this->height >> i);
}

void Texture2D::read(BinaryReader& reader)
{
	int32_t surface_format_i = reader.ReadInt32();
	this->surface_format = static_cast<Texture2D_SurfaceFormat>(surface_format_i);
	this->width = reader.ReadUInt32();
	this->height = reader.ReadUInt32();
	uint32_t mip_count = reader.ReadUInt32();

	switch(surface_format)
	{
		case Texture2D_SurfaceFormat::RGBA8888:
		{
			break;
		}
		default:
		{
			throw ("unsupported surface format: " + std::to_string(surface_format_i));
		}
	}

	for(uint_fast32_t i = 0; i < mip_count; ++i)
	{
		uint32_t mip_size = reader.ReadUInt32();
		std::unique_ptr<uint8_t[]> mip_data(reader.ReadBytes(mip_size));
		std::vector<uint8_t> mip_data_vec(mip_data.get(), mip_data.get() + mip_size);
		this->mips.push_back(mip_data_vec);
	}
}

Sound::Sound(BinaryReader& reader)
{
	this->type_reader_name = "Microsoft.Xna.Framework.Content.SoundEffectReader";
	this->read(reader);
}

void Sound::read(BinaryReader& reader)
{
	uint32_t format_size = reader.ReadUInt32();
	if(format_size != 18)
	{
		throw ("unhandled format header size: " + std::to_string(format_size));
	}

	uint16_t format_i = reader.ReadUInt16();
	this->format = static_cast<SoundFormat>(format_i);
	if(this->format != SoundFormat::PCM)
	{
		throw ("unhandled sound format: " + std::to_string(format_i));
	}

	// see https://msdn.microsoft.com/en-us/library/windows/desktop/dd390970%28v=vs.85%29.aspx
	this->channel_count = reader.ReadUInt16();
	this->sample_rate = reader.ReadUInt32();
	this->average_byte_rate = reader.ReadUInt32();
	this->block_align = reader.ReadUInt16();
	this->bits_per_sample = reader.ReadUInt16();
	if(bits_per_sample % 8 != 0)
	{
		throw ("bits per sample is not a multiple of 8: " + std::to_string(bits_per_sample));
	}
	uint16_t bytes_per_sample = bits_per_sample / 8;

	if(average_byte_rate != sample_rate * channel_count * bytes_per_sample)
	{
		throw ("average_byte_rate does not match sample_rate * channel_count * bits_per_sample / 8");
	}

	if(block_align != channel_count * bytes_per_sample)
	{
		throw ("block_align does not match channel_count * bits_per_sample / 8");
	}

	uint16_t extra_info_size = reader.ReadUInt16();
	if(extra_info_size != 0)
	{
		throw ("extra info size is " + std::to_string(extra_info_size));
	}

	uint32_t data_size = reader.ReadUInt32();
	std::unique_ptr<uint8_t[]> data_ptr(reader.ReadBytes(data_size));
	this->data = std::vector<uint8_t>(data_ptr.get(), data_ptr.get() + data_size);

	uint32_t loop_start = reader.ReadUInt32();
	uint32_t loop_end = reader.ReadUInt32();
	uint32_t duration_ms = reader.ReadUInt32();
	// TODO
}



} // namespace Content
} // namespace XNA