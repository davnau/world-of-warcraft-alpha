using System.Buffers.Binary;
using System.IO.Compression;
using System.Numerics;
using System.Text;
using System.Text.Json;
using BattleNet.Core.Configuration;
using BattleNet.Persistence;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;
using BattleNet.Server;

static void Assert(bool condition, string message)
{
    if (!condition)
        throw new InvalidOperationException(message);
}

var packet = PacketWriter.BuildPacket(Opcode.SMSG_PONG, [1, 2, 3, 4]);
Assert(BinaryPrimitives.ReadUInt16BigEndian(packet.AsSpan(0, 2)) == 8, "Unexpected world header length.");
Assert(BinaryPrimitives.ReadUInt32LittleEndian(packet.AsSpan(2, 4)) == (uint)Opcode.SMSG_PONG, "Unexpected opcode bytes.");

var salt = Enumerable.Range(0, Srp6.KeySize).Select(static value => (byte)value).ToArray();
var verifier = Srp6.CalculatePasswordVerifier("TEST", "PASSWORD", salt);
var serverPrivateKey = Enumerable.Repeat((byte)7, Srp6.KeySize).ToArray();
var serverPublicKey = Srp6.CalculateServerPublicKey(verifier, serverPrivateKey);
Assert(Convert.ToHexString(verifier).Equals(
    "96853714EA7F8333565E2DA8AEE5C435EC130B1189F25663A62386E0DC0BC351",
    StringComparison.OrdinalIgnoreCase), "SRP6 verifier does not match Alpha Core.");
Assert(Convert.ToHexString(serverPublicKey).Equals(
    "DFA09AD6D33F497D908BFCEC97D0C8B799F616A2A68041F0530EBE2E486AB242",
    StringComparison.OrdinalIgnoreCase), "SRP6 public key does not match Alpha Core.");
Assert(serverPublicKey.Length == Srp6.KeySize, "Unexpected SRP6 public key size.");
Assert(Srp6.FromLittleEndian(serverPublicKey) < Srp6.Modulus, "SRP6 public key is not reduced.");
Assert(Convert.ToHexString(Srp6.CalculateInterleaved(Enumerable.Range(0, 32).Select(static value => (byte)value).ToArray())).Equals(
    "9F32F1E4F14DF7375D78E0A1D04857BADA15D3D91E3BA8100472B55F54C99C703E7592B8E38CB19C",
    StringComparison.OrdinalIgnoreCase), "SRP6 interleaving does not match Alpha Core.");
Assert(PacketWriter.StringToBytes("BattleNet").Last() == 0, "CString is not terminated.");

var initialSpells = InitialSpellPacketWriter.Build(
    [
        new CharacterSpellRecord(1337, Active: true, Disabled: false),
        new CharacterSpellRecord(1338, Active: false, Disabled: false)
    ],
    new Dictionary<uint, int> { [1337] = 4 });
Assert(initialSpells.Length == 9, "Initial spell payload has the wrong Alpha layout.");
Assert(BinaryPrimitives.ReadUInt16LittleEndian(initialSpells.AsSpan(1, 2)) == 2,
    "Initial spell count must include inactive persisted rows.");
Assert(BinaryPrimitives.ReadInt16LittleEndian(initialSpells.AsSpan(3, 2)) == 1337,
    "Initial spell ID is in the wrong position.");
Assert(BinaryPrimitives.ReadInt16LittleEndian(initialSpells.AsSpan(5, 2)) == 4,
    "Initial spell action-bar index is in the wrong position.");

var actionButtons = ActionButtonPacketWriter.Build(new Dictionary<int, int> { [3] = 42 });
Assert(actionButtons.Length == 120 * sizeof(int), "Action-bar payload does not contain 120 slots.");
Assert(BinaryPrimitives.ReadInt32LittleEndian(actionButtons.AsSpan(3 * sizeof(int), sizeof(int))) == 42,
    "Action-bar slot was not serialized.");

var itemTemplate = new ItemTemplateRecord(
    42,
    0,
    0,
    "Test Item",
    "Test description",
    123,
    1,
    0,
    10,
    2,
    0,
    -1,
    -1,
    1,
    0,
    0,
    0,
    0,
    1,
    0,
    Enumerable.Repeat(new ItemStatRecord(0, 0), 10).ToArray(),
    Enumerable.Repeat(new ItemDamageRecord(0, 0, 0), 5).ToArray(),
    0,
    0,
    0,
    0,
    0,
    0,
    1000,
    0,
    Enumerable.Repeat(new ItemSpellRecord(0, 0, 0, -1, 0, -1), 5).ToArray(),
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0);
var itemQueryPayload = ItemQueryPacketWriter.BuildSinglePayload(itemTemplate);
Assert(BinaryPrimitives.ReadUInt32LittleEndian(itemQueryPayload.AsSpan(0, 4)) == 42,
    "Single item query response does not start with the item entry.");
var itemDisplayOffset = sizeof(uint) * 3
    + PacketWriter.StringToBytes(itemTemplate.Name).Length
    + 3; // The remaining three localized display-name strings are empty.
Assert(BinaryPrimitives.ReadUInt32LittleEndian(itemQueryPayload.AsSpan(itemDisplayOffset, sizeof(uint))) == 123,
    "Single item query response does not place display ID after exactly four display-name strings.");
var itemQueryBatch = ItemQueryPacketWriter.BuildMultiplePayloads([itemTemplate]);
Assert(itemQueryBatch.Count == 1 && itemQueryBatch[0][0] == 1,
    "Multiple item query response does not contain its item count.");
Assert(itemQueryBatch[0].AsSpan(1).SequenceEqual(itemQueryPayload),
    "Multiple item query response item bytes differ from single response data.");
var attackStartPayload = CombatPacketWriter.BuildAttackStart(7, 0xF130000000000001UL);
Assert(attackStartPayload.Length == sizeof(ulong) * 2
    && BinaryPrimitives.ReadUInt64LittleEndian(attackStartPayload.AsSpan(sizeof(ulong))) == 0xF130000000000001UL,
    "Attack start payload does not contain the creature target.");
var attackerStatePayload = CombatPacketWriter.BuildAttackerStateUpdate(7, 0xF130000000000001UL, 12);
Assert(attackerStatePayload.Length == 61 && BinaryPrimitives.ReadUInt32LittleEndian(attackerStatePayload.AsSpan(20)) == 12,
    "Attacker state update payload has the wrong Alpha layout.");

var player = new CharacterSummary(
    7,
    "Testhero",
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    12,
    0,
    1.0f,
    2.0f,
    3.0f,
    Orientation: 1.5f);
var playerUpdatePayload = PlayerUpdatePacketWriter.Build(player);
var deadPlayerRuntime = new PlayerRuntimeState(
    player with { Health = 0 },
    Array.Empty<CharacterSpellRecord>());
Assert(deadPlayerRuntime.Health == 0,
    "A persisted dead character was revived as legacy placeholder data.");
var playerUpdatePacket = PacketWriter.BuildPacket(Opcode.SMSG_UPDATE_OBJECT, playerUpdatePayload);
Assert(BinaryPrimitives.ReadUInt32LittleEndian(playerUpdatePacket.AsSpan(2, 4)) == (uint)Opcode.SMSG_COMPRESSED_UPDATE_OBJECT,
    "Large player CREATE_OBJECT update was not compressed.");
Assert(BinaryPrimitives.ReadUInt32LittleEndian(playerUpdatePacket.AsSpan(6, 4)) == (uint)playerUpdatePayload.Length,
    "Compressed update original length is incorrect.");
using (var compressed = new ZLibStream(
           new MemoryStream(playerUpdatePacket.AsMemory(10).ToArray()),
           CompressionMode.Decompress))
using (var decompressed = new MemoryStream())
{
    compressed.CopyTo(decompressed);
    var update = decompressed.ToArray();
    Assert(BinaryPrimitives.ReadUInt32LittleEndian(update.AsSpan(0, 4)) == 1,
        "Player CREATE_OBJECT update transaction count is incorrect.");
}

var inventoryItem = new CharacterInventoryRecord(
    10,
    player.Guid,
    0,
    23,
    23,
    42,
    1);
var inventoryPayload = InventoryPacketWriter.Build(inventoryItem, player);
var inventoryPacket = PacketWriter.BuildPacket(Opcode.SMSG_UPDATE_OBJECT, inventoryPayload);
Assert(BinaryPrimitives.ReadUInt32LittleEndian(inventoryPacket.AsSpan(2, 4)) == (uint)Opcode.SMSG_COMPRESSED_UPDATE_OBJECT,
    "Inventory CREATE_OBJECT update was not compressed.");
Assert(BinaryPrimitives.ReadUInt32LittleEndian(inventoryPacket.AsSpan(6, 4)) == (uint)inventoryPayload.Length,
    "Inventory compressed update original length is incorrect.");
using (var compressed = new ZLibStream(
           new MemoryStream(inventoryPacket.AsMemory(10).ToArray()),
           CompressionMode.Decompress))
using (var decompressed = new MemoryStream())
{
    compressed.CopyTo(decompressed);
    var update = decompressed.ToArray();
    Assert(BinaryPrimitives.ReadUInt32LittleEndian(update.AsSpan(0, 4)) == 1,
        "Inventory CREATE_OBJECT update transaction count is incorrect.");
}

var corpse = new PlayerCorpseRuntime(
    0xF101000000000001UL,
    player.Guid,
    player.Map,
    player.PositionX,
    player.PositionY,
    player.PositionZ,
    player.Orientation,
    49,
    1.0f,
    0x00010000,
    0,
    0,
    1,
    new uint[19],
    DateTimeOffset.UtcNow.AddMinutes(10));
var corpsePayload = CorpseUpdatePacketWriter.Build(corpse);
Assert(corpsePayload.Length == 255, "Corpse CREATE_OBJECT payload has the wrong Alpha size.");
Assert(corpsePayload[13] == 7, "Corpse CREATE_OBJECT uses the wrong object type.");
Assert(corpsePayload[102] == 2, "Corpse update mask does not contain two blocks.");
const int corpseFieldsOffset = 111;
Assert(BinaryPrimitives.ReadUInt32LittleEndian(corpsePayload.AsSpan(corpseFieldsOffset + (2 * 4), 4)) == 0x81,
    "Corpse object type mask is incorrect.");
Assert(BinaryPrimitives.ReadUInt64LittleEndian(corpsePayload.AsSpan(corpseFieldsOffset + (6 * 4), 8)) == player.Guid,
    "Corpse owner field is incorrect.");
Assert(BinaryPrimitives.ReadUInt32LittleEndian(corpsePayload.AsSpan(corpseFieldsOffset + (12 * 4), 4)) == 49,
    "Corpse display field is incorrect.");

var teleportPayload = PlayerTeleportPacketWriter.Build(10.0f, 20.0f, 30.0f, 1.25f);
Assert(teleportPayload.Length == 48, "Near-teleport payload is not the recovered <Q9fI> layout.");
Assert(BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(teleportPayload.AsSpan(24, 4))) == 10.0f
    && BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(teleportPayload.AsSpan(36, 4))) == 1.25f,
    "Near-teleport destination fields are misaligned.");

var realmRepository = new InMemoryRealmRepository();
var dbcRepository = new InMemoryDbcRepository();
var humanRace = await dbcRepository.GetRaceDefinitionAsync(1);
Assert(humanRace is not null && humanRace.MaleDisplayId == 49 && humanRace.FemaleDisplayId == 50,
    "Alpha ChrRaces fallback data is incorrect.");

var createdCharacter = await realmRepository.CreateCharacterAsync(new CharacterCreateRequest(
    42,
    1,
    "Testhero",
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    StartingLocation.Default));
Assert(createdCharacter is not null && createdCharacter.Name == "Testhero", "Character creation failed.");
Assert((await realmRepository.GetCharactersAsync(42, 1)).Count == 1, "Character enumeration failed.");
await realmRepository.InitializeCharacterStartupStateAsync(
    createdCharacter!.Guid,
    new CharacterStartupState(
        [1337],
        new Dictionary<int, int> { [3] = 42 },
        [new CharacterInventoryRecord(0, 0, 0, 23, 23, 42, 1)]));
Assert((await realmRepository.GetCharacterSpellsAsync(createdCharacter.Guid)).Single().SpellId == 1337,
    "Character starter spell state was not persisted in memory.");
Assert((await realmRepository.GetActionButtonsAsync(createdCharacter.Guid))[3] == 42,
    "Character starter action state was not persisted in memory.");
var storedInventory = (await realmRepository.GetCharacterInventoryAsync(createdCharacter.Guid)).Single();
Assert(storedInventory.Guid != 0 && storedInventory.Owner == createdCharacter.Guid,
    "Character starter inventory state was not assigned an item owner/guid.");
Assert(await realmRepository.DeleteCharacterAsync(42, 1, createdCharacter!.Guid), "Character deletion failed.");
Assert((await realmRepository.GetCharactersAsync(42, 1)).Count == 0, "Deleted character is still enumerable.");

var fileTablePath = Path.Combine(Path.GetTempPath(), "BattleNetTables-" + Guid.NewGuid().ToString("N"));
try
{
    var fileRepositories = new FileRepositories(new ServerOptions { DataDirectory = fileTablePath });
    Assert(await fileRepositories.FindAccountAsync("TEST") is not null,
        "Self-contained tables did not create the default local account.");
    Assert(await fileRepositories.CreateAccountAsync("NewPlayer", "NewPassword1!") == AccountCreateResult.Success,
        "Self-contained account registration failed.");
    Assert(await fileRepositories.CreateAccountAsync("newplayer", "DifferentPassword1!") == AccountCreateResult.AlreadyExists,
        "Self-contained account registration allowed a duplicate account name.");
    var registeredAccount = await fileRepositories.FindAccountAsync("NEWPLAYER");
    Assert(registeredAccount is not null
        && registeredAccount.SaltHex.Length == Srp6.KeySize * 2
        && registeredAccount.VerifierHex.Length == Srp6.KeySize * 2,
        "Registered account does not contain Alpha-compatible SRP6 credentials.");
    Assert(await fileRepositories.CreateAccountAsync("bad name", "NewPassword1!") == AccountCreateResult.InvalidUsername,
        "Self-contained account registration accepted an invalid account name.");
    Assert(await fileRepositories.CreateAccountAsync("VALIDNAME", "short") == AccountCreateResult.InvalidPassword,
        "Self-contained account registration accepted an invalid password.");
    var fileCharacter = await fileRepositories.CreateCharacterAsync(new CharacterCreateRequest(
        1,
        1,
        "Filehero",
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        StartingLocation.Default));
    Assert(fileCharacter is not null, "Self-contained character table insert failed.");
    await fileRepositories.InitializeCharacterStartupStateAsync(
        fileCharacter!.Guid,
        new CharacterStartupState(
            [1337],
            new Dictionary<int, int> { [0] = 6603 },
            [new CharacterInventoryRecord(0, 0, 0, 23, 23, 42, 1)]));
    Assert((await fileRepositories.GetCharacterSpellsAsync(fileCharacter.Guid)).Single().SpellId == 1337,
        "Self-contained character spell table insert failed.");
    var fileQuestRepository = (IQuestRepository)fileRepositories;
    Assert(await fileQuestRepository.AcceptCharacterQuestAsync(new CharacterQuestRecord(fileCharacter.Guid, 33)),
        "Self-contained character quest insert failed.");
    Assert((await fileQuestRepository.GetCharacterQuestsAsync(fileCharacter.Guid)).Single().QuestId == 33,
        "Self-contained character quest lookup failed.");
    Assert(await fileQuestRepository.RemoveCharacterQuestAsync(fileCharacter.Guid, 33),
        "Self-contained character quest removal failed.");
    Assert(await fileQuestRepository.AcceptCharacterQuestAsync(new CharacterQuestRecord(fileCharacter.Guid, 7, ObjectiveCounts: [0])),
        "Self-contained objective quest insert failed.");
    Assert(await fileQuestRepository.UpdateCharacterQuestAsync(
        new CharacterQuestRecord(fileCharacter.Guid, 7, ObjectiveCounts: [3])),
        "Self-contained quest objective update failed.");
    Assert((await fileQuestRepository.GetCharacterQuestsAsync(fileCharacter.Guid)).Single().Progress[0] == 3,
        "Self-contained quest objective progress was not persisted.");
    Assert(await fileQuestRepository.RemoveCharacterQuestAsync(fileCharacter.Guid, 7),
        "Self-contained objective quest removal failed.");
    File.WriteAllText(
        Path.Combine(fileTablePath, "item_template.json"),
        JsonSerializer.Serialize(new[] { itemTemplate }));
    Assert(await fileRepositories.GetItemTemplateAsync(42) is not null,
        "Self-contained test item template could not be reloaded.");
    var fileMutationRepository = (ICharacterMutationRepository)fileRepositories;
    var mutation = await fileMutationRepository.ApplyInventoryMutationAsync(
        fileCharacter.Guid,
        [new InventoryItemDelta(42, 2)],
        [new InventoryItemDelta(42, 1)],
        moneyDelta: 7);
    Assert(mutation is not null,
        "Self-contained inventory reward mutation returned no result.");
    Assert(mutation!.AddedItems.Count == 2 && mutation.Money == 7,
        $"Self-contained inventory reward mutation was not applied atomically (added={mutation.AddedItems.Count}, money={mutation.Money}).");
    Assert((await fileRepositories.FindCharacterAsync(fileCharacter.Guid, 1))!.Money == 7,
        "Self-contained character money mutation was not persisted.");
}
finally
{
    if (Directory.Exists(fileTablePath))
        Directory.Delete(fileTablePath, recursive: true);
}

var populatedTablesPath = Environment.GetEnvironmentVariable("BATTLE_NET_TEST_TABLES");
if (!string.IsNullOrWhiteSpace(populatedTablesPath) && Directory.Exists(populatedTablesPath))
{
    var populatedRepositories = new FileRepositories(new ServerOptions { DataDirectory = populatedTablesPath });
    var populatedWorld = new FileWorldContentRepository(populatedTablesPath);
    Assert((await populatedRepositories.GetStartingSpellIdsAsync(1, 1)).Count > 20,
        "Populated Alpha starting-spell table was not loaded.");
    Assert((await populatedRepositories.GetStartingItemRecordsAsync(1, 1)).Count > 0,
        "Populated Alpha starting-item table was not loaded.");
    Assert(await populatedRepositories.GetItemTemplateAsync(38) is not null,
        "Populated Alpha item_template table was not loaded.");
    Assert((await populatedRepositories.GetStartingOutfitItemIdsAsync(1, 1, 0)).Count > 0,
        "Populated Alpha CharStartOutfit table was not loaded.");
    var creatureTemplate = await populatedWorld.GetCreatureTemplateAsync(196);
    Assert(creatureTemplate is not null && creatureTemplate.Name.Length > 0,
        "Populated Alpha creature_template table was not loaded.");
    var gameObjectTemplate = await populatedWorld.GetGameObjectTemplateAsync(31);
    Assert(gameObjectTemplate is not null && gameObjectTemplate.Data.Count == 10,
        "Populated Alpha gameobject_template table was not loaded.");
    var questTemplate = await populatedWorld.GetQuestTemplateAsync(33);
    Assert(questTemplate is not null && questTemplate.Title.Length > 0,
        "Populated Alpha quest_template table was not loaded.");
    var nearbyCreatures = await populatedWorld
        .GetCreatureSpawnsNearAsync(0, -8949.95f, -132.493f, 83.5312f, 250, 128);
    Assert(nearbyCreatures.Count > 0,
        "Populated Alpha creature spawn table was not loaded.");
    var nearbyGameObjects = await populatedWorld
        .GetGameObjectSpawnsNearAsync(0, -8949.95f, -132.493f, 83.5312f, 250, 128);
    Assert(nearbyGameObjects.Count > 0,
        "Populated Alpha gameobject spawn table was not loaded.");
    var nearbyGameObjectTemplate = await populatedWorld.GetGameObjectTemplateAsync(nearbyGameObjects[0].Entry);
    Assert(nearbyGameObjectTemplate is not null && nearbyGameObjectTemplate.Scale > 0,
        "Populated Alpha gameobject display data was not loaded.");
    var questGiver = nearbyCreatures
        .Select(spawn => populatedWorld.GetCreatureTemplateAsync(spawn.Entry).AsTask().GetAwaiter().GetResult())
        .FirstOrDefault(template => template is not null && template.NpcFlags != 0);
    Assert(questGiver is not null, "Populated Alpha NPC flags were not loaded.");
    Assert((await populatedWorld.GetCreatureQuestIdsAsync(196, starter: true)).Contains(33u),
        "Populated Alpha creature quest-starter relation was not loaded.");
    Assert((await populatedWorld.GetCreatureQuestIdsAsync(196, starter: false)).Contains(33u),
        "Populated Alpha creature quest-finisher relation was not loaded.");
    var vendorItems = await populatedWorld.GetVendorItemsAsync(54, 0);
    Assert(vendorItems.Count > 0 && vendorItems[0].ItemEntry == 2488,
        "Populated Alpha NPC vendor table was not loaded.");
    var questListPayload = QuestGiverPacketWriters.BuildList(
        0xF130000000000001UL,
        string.Empty,
        0,
        [new WorldQuestGiverQuest(33, QuestGiverPacketWriters.Quest, questTemplate!.QuestLevel, questTemplate.Title)]);
    Assert(BinaryPrimitives.ReadUInt64LittleEndian(questListPayload) == 0xF130000000000001UL,
        "Quest giver list payload does not begin with the NPC GUID.");
    var questDetailsPayload = QuestGiverPacketWriters.BuildDetails(0xF130000000000001UL, questTemplate);
    Assert(BinaryPrimitives.ReadUInt32LittleEndian(questDetailsPayload.AsSpan(sizeof(ulong))) == 33,
        "Quest giver details payload does not contain the quest entry.");
    var vendorPayload = VendorPacketWriter.BuildInventory(0xF130000000000001UL, vendorItems);
    Assert(BinaryPrimitives.ReadUInt64LittleEndian(vendorPayload) == 0xF130000000000001UL
        && vendorPayload[sizeof(ulong)] == vendorItems.Count,
        "Vendor inventory payload does not contain its GUID and item count.");
    var gameObjectPayload = GameObjectUpdatePacketWriter.Build(nearbyGameObjects[0], nearbyGameObjectTemplate!);
    Assert(BinaryPrimitives.ReadUInt32LittleEndian(gameObjectPayload.AsSpan(0, sizeof(uint))) == 1,
        "Gameobject CREATE_OBJECT update transaction count is incorrect.");
    var fireball = await populatedWorld.GetSpellDefinitionAsync(133);
    Assert(fireball is not null && fireball.Effects.Any(effect => effect.EffectType == 2) && fireball.MaxRange > 0,
        "Extracted Alpha Spell.dbc combat data was not loaded.");
    var aiEvents = await populatedWorld.GetCreatureAiEventsAsync(1978);
    Assert(aiEvents.Count > 0 && aiEvents.Any(eventRow => eventRow.EventType == 4),
        "Creature EventAI event data was not loaded.");
    var aiScripts = await populatedWorld.GetCreatureAiScriptsAsync(
        aiEvents.SelectMany(eventRow => new[] { eventRow.Action1Script, eventRow.Action2Script, eventRow.Action3Script })
            .Where(scriptId => scriptId > 0)
            .Distinct()
            .ToArray());
    Assert(aiScripts.Count > 0,
        "Creature EventAI script commands were not loaded.");
    var broadcastText = await populatedWorld.GetBroadcastTextAsync(1);
    Assert(broadcastText is not null && broadcastText.MaleText.Length > 0,
        "Broadcast text data for EventAI talk commands was not loaded.");
    var creatureSpells = await populatedWorld.GetCreatureSpellsAsync(400);
    Assert(creatureSpells.Count > 0 && creatureSpells[0].SpellId == 6016,
        "Populated Alpha creature spell-list data was not loaded.");
    var movement = await populatedWorld.GetCreatureWaypointsAsync(51, 225);
    Assert(movement.Count > 0 && movement[0].Point == 1,
        "Populated Alpha creature waypoint data was not loaded.");
    var loot = await populatedWorld.GetCreatureLootAsync(3);
    Assert(loot.Count > 0 && loot[0].ItemEntry == 804,
        "Populated Alpha creature loot data was not loaded.");
    Assert(MonsterMovePacketWriter.Build(0xF130000000000001UL, 1, 2, 3, 4, 5, 6, 1000).Length > 40,
        "Monster movement packet was not serialized.");
}

Console.WriteLine("BattleNet protocol and persistence self-tests passed.");
