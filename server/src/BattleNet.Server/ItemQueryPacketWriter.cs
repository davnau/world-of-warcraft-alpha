using System.IO;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;

namespace BattleNet.Server;

/// <summary>
/// Serializes the item query data emitted by Alpha's ObjectQueryUtils.
/// </summary>
internal static class ItemQueryPacketWriter
{
    private const int ItemQueryBatchLimit = 255;

    public static byte[] BuildSinglePayload(ItemTemplateRecord template) => BuildQueryData(template);

    public static IReadOnlyList<byte[]> BuildMultiplePayloads(IEnumerable<ItemTemplateRecord> templates)
    {
        var batches = new List<byte[]>();
        using var queryData = new MemoryStream();
        var itemCount = 0;

        foreach (var template in templates)
        {
            var itemBytes = BuildQueryData(template);
            var exceedsPacketLimit = itemCount > 0
                && PacketWriter.HeaderSize + sizeof(uint) + queryData.Length + itemBytes.Length > PacketWriter.MaximumPacketSize;
            if (itemCount == ItemQueryBatchLimit || exceedsPacketLimit)
            {
                batches.Add(BuildMultiplePayload(queryData, itemCount));
                queryData.SetLength(0);
                itemCount = 0;
            }

            queryData.Write(itemBytes);
            itemCount++;
        }

        if (itemCount > 0)
            batches.Add(BuildMultiplePayload(queryData, itemCount));
        return batches;
    }

    private static byte[] BuildMultiplePayload(MemoryStream queryData, int itemCount)
    {
        var payload = new byte[checked(sizeof(byte) + (int)queryData.Length)];
        payload[0] = checked((byte)itemCount);
        queryData.Position = 0;
        queryData.ReadExactly(payload.AsSpan(sizeof(byte)));
        return payload;
    }

    private static byte[] BuildQueryData(ItemTemplateRecord template)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(template.Entry);
        writer.Write(unchecked((uint)template.Class));
        writer.Write(unchecked((uint)template.Subclass));
        WriteCString(writer, template.Name);
        writer.Write((byte)0);
        writer.Write((byte)0);
        writer.Write((byte)0);
        writer.Write(template.DisplayId);
        writer.Write(template.Quality);
        writer.Write(template.Flags);
        writer.Write(template.BuyPrice);
        writer.Write(template.SellPrice);
        writer.Write(template.InventoryType);
        writer.Write(template.AllowableClass);
        writer.Write(template.AllowableRace);
        writer.Write(template.ItemLevel);
        writer.Write(template.RequiredLevel);
        writer.Write(template.RequiredSkill);
        writer.Write(template.RequiredSkillRank);
        writer.Write(template.MaxCount);
        writer.Write(template.Stackable);
        writer.Write(template.ContainerSlots);

        for (var index = 0; index < 10; index++)
        {
            var stat = index < template.Stats.Count ? template.Stats[index] : new ItemStatRecord(0, 0);
            writer.Write(stat.Type);
            writer.Write(stat.Value);
        }

        for (var index = 0; index < 5; index++)
        {
            var damage = index < template.Damages.Count
                ? template.Damages[index]
                : new ItemDamageRecord(0, 0, 0);
            writer.Write((int)damage.Minimum);
            writer.Write((int)damage.Maximum);
            writer.Write(damage.Type);
        }

        writer.Write(template.Armor);
        writer.Write(template.HolyResistance);
        writer.Write(template.FireResistance);
        writer.Write(template.NatureResistance);
        writer.Write(template.FrostResistance);
        writer.Write(template.ShadowResistance);
        writer.Write(checked((uint)template.Delay));
        writer.Write(template.AmmoType);
        writer.Write(0u); // Durability, not implemented by Alpha's client.

        for (var index = 0; index < 5; index++)
        {
            var spell = index < template.Spells.Count
                ? template.Spells[index]
                : new ItemSpellRecord(0, 0, 0, -1, 0, -1);
            writer.Write(spell.SpellId);
            writer.Write(spell.Trigger);
            writer.Write(spell.Charges);
            writer.Write(spell.Cooldown);
            writer.Write(spell.Category);
            writer.Write(spell.CategoryCooldown);
        }

        writer.Write(template.Bonding);
        WriteCString(writer, template.Description);
        writer.Write(template.PageText);
        writer.Write(template.PageLanguage);
        writer.Write(template.PageMaterial);
        writer.Write(template.StartQuest);
        writer.Write(template.LockId);
        writer.Write(template.Material);
        writer.Write(template.Sheath);
        return stream.ToArray();
    }

    private static void WriteCString(BinaryWriter writer, string value) =>
        writer.Write(PacketWriter.StringToBytes(value));
}
