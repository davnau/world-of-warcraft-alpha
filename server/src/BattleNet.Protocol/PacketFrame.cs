namespace BattleNet.Protocol;

public readonly record struct PacketFrame(Opcode Opcode, byte[] Payload)
{
    public int PayloadLength => Payload.Length;
}
