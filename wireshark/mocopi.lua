local mocopi_proto = Proto("mocopi","mocopi UDP","sony motion format")
mocopi_proto.fields = {}

mocopi_proto.fields.raw_length = ProtoField.new("Len","mocopi_proto.raw_length",ftypes.UINT32)
mocopi_proto.fields.CC = ProtoField.string("mocopi_proto.CC","4CC", base.ASCII)
mocopi_proto.fields.raw = ProtoField.bytes("mocopi_proto.RAW","RAW", base.NONE)

local parents = {
    ["head"]=true,
    ["sndf"]=true,
    ["skdf"]=true,
    ["bons"]=true,
    ["bndt"]=true,
    ["fram"]=true,
    ["btrs"]=true,
    ["btdt"]=true,
    ["NEVER"]=false
}

function recursive_parse(buffer, pinto, parent)
    local offset=0
    local total_len=buffer:len()

    while offset<total_len do
        local internal_len = buffer(offset+0, 4):le_uint()
        local cc           = buffer(offset+4, 4):string()
        local raw          = buffer(offset+8, internal_len)

        local child = parent:add(mocopi_proto, buffer(offset, internal_len), cc)
        child:add_le(mocopi_proto.fields.raw_length, buffer(offset+0, 4))
        child:add   (mocopi_proto.fields.CC,         buffer(offset+4, 4))
        if parents[cc] then
            recursive_parse(buffer(offset+8, internal_len), pinfo, child)
        else
            child:add   (mocopi_proto.fields.raw,        buffer(offset+8, internal_len))
        end

        offset = offset+internal_len+8
    end
end

function mocopi_proto.dissector(buffer, pinfo, tree)
    pinfo.cols.protocol = "mocopi"
    local subtree = tree:add(mocopi_proto, buffer(), "mocopi UDP")
    recursive_parse(buffer(), pinfo, subtree)
end

udp_table = DissectorTable.get("udp.port")
udp_table:add(12351, mocopi_proto)
