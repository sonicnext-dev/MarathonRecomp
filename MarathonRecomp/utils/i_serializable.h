#pragma once

template <typename TReader, typename TWriter>
class ISerializable
{
public:
    virtual ~ISerializable() = default;
    virtual bool Read(const TReader& in_rReader) = 0;
    virtual bool Write(TWriter& in_rWriter) = 0;
};
