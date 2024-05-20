//
// Created by plaka on 17.03.2023.
//

#ifndef LIVERUSSIA_STREAMINGINFO_H
#define LIVERUSSIA_STREAMINGINFO_H

#include <cstdint>

enum eStreamingFlags {
    STREAMING_DEFAULT = 0x0,
    STREAMING_UNKNOWN_1 = 0x1,
    STREAMING_GAME_REQUIRED = 0x2,
    STREAMING_MISSION_REQUIRED = 0x4,
    STREAMING_KEEP_IN_MEMORY = 0x8,
    STREAMING_PRIORITY_REQUEST = 0x10,
    STREAMING_LOADING_SCENE = 0x20,
    STREAMING_DONTREMOVE_IN_LOADSCENE = STREAMING_LOADING_SCENE | STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED | STREAMING_GAME_REQUIRED,
};

enum eStreamingLoadState : uint8_t {
    // Model isn't loaded
    LOADSTATE_NOT_LOADED = 0,

    // Model is loaded
    LOADSTATE_LOADED = 1,

    // Model in request list, but not yet in loading channel (TODO: Verify this)
    LOADSTATE_REQUESTED = 2,

    // Model is being read
    LOADSTATE_READING = 3,

    // If the model is a `big` one this state is used to indicate
    // that the model's first half has been loaded and is yet to be
    // finished by loading the second half.
    // When it has been loaded the state is set to `LOADED`
    LOADSTATE_FINISHING = 4
};

constexpr auto STREAMING_SECTOR_SIZE = 2048u;

class CStreamingInfo {
public:
    int16_t m_nNextIndex;     // ms_pArrayBase array index
    int16_t m_nPrevIndex;     // ms_pArrayBase array index
    int16_t m_nNextIndexOnCd; // ModelId after this file in the containing image file
    union {
        uint8_t m_nFlags; // see eStreamingFlags
        struct {
            uint8_t bUnkn0x1 : 1;
            uint8_t bGameRequired : 1;
            uint8_t bMissionRequired : 1;
            uint8_t bKeepInMemory : 1;
            uint8_t bPriorityRequest : 1;
            uint8_t bLoadingScene : 1;
        };
    };
    uint8_t  m_nImgId;        // Index into CStreaming::ms_files
    uint32_t m_nCdPosn;       // Position in directory (in sectors)
    size_t m_nCdSize;       // Size of resource (in sectors); m_nCdSize * STREAMING_BLOCK_SIZE = actual size in bytes
    eStreamingLoadState m_nLoadState;

    static inline CStreamingInfo* ms_pArrayBase;

public:
    void Init();
    [[nodiscard]] size_t GetCdPosn() const;
    void SetCdPosnAndSize(size_t CdPosn, size_t CdSize);
    bool GetCdPosnAndSize(size_t& CdPosn, size_t& CdSize);
    [[nodiscard]] bool HasCdPosnAndSize() const noexcept { return m_nCdSize != 0; }
    [[nodiscard]] auto GetCdSize() const { return m_nCdSize; }
    CStreamingInfo* GetNext() { return m_nNextIndex == -1 ? nullptr : &ms_pArrayBase[m_nNextIndex]; }
    CStreamingInfo* GetPrev() { return m_nPrevIndex == -1 ? nullptr : &ms_pArrayBase[m_nPrevIndex]; }

    /*!
    * @addr 0x407480
    * @brief Insert `*this` after the item `*after`
    */
    void AddToList(CStreamingInfo* after);

    /*!
    * @addr 0x4074E0
    * @brief Remove `*this` from it's current list
    */
    void RemoveFromList();

    /*!
    * @notsa
    * @brief Check if `*this` is in any list
    */
    bool InList() const;

    void SetFlags(uint32_t flags) { m_nFlags |= flags; }
    void ClearFlags(uint32_t flags) { m_nFlags &= ~flags; }
    [[nodiscard]] auto GetFlags() const noexcept { return m_nFlags; }
    void ClearAllFlags() noexcept { m_nFlags = 0; } // Clears all flags
    [[nodiscard]] bool AreAnyFlagsSetOutOf(uint32_t flags) const noexcept { return GetFlags() & flags; }

    [[nodiscard]] bool IsLoadedOrBeingRead() const noexcept {
        switch (m_nLoadState) {
            case eStreamingLoadState::LOADSTATE_LOADED:
            case eStreamingLoadState::LOADSTATE_READING:
                return true;
            default:
                return false;
        }
    }
    [[nodiscard]] bool IsLoaded() const { return m_nLoadState == eStreamingLoadState::LOADSTATE_LOADED; }
    [[nodiscard]] bool IsRequested() const { return m_nLoadState == eStreamingLoadState::LOADSTATE_REQUESTED; }
    [[nodiscard]] bool IsBeingRead() const { return m_nLoadState == eStreamingLoadState::LOADSTATE_READING; }
    [[nodiscard]] bool IsLoadingFinishing() const { return m_nLoadState == eStreamingLoadState::LOADSTATE_FINISHING; }

    [[nodiscard]] bool DontRemoveInLoadScene() const noexcept { return m_nFlags & eStreamingFlags::STREAMING_DONTREMOVE_IN_LOADSCENE; }
    [[nodiscard]] bool IsGameRequired() const noexcept        { return m_nFlags & eStreamingFlags::STREAMING_GAME_REQUIRED; }
    [[nodiscard]] bool IsMissionRequired() const noexcept     { return m_nFlags & eStreamingFlags::STREAMING_MISSION_REQUIRED; }
    [[nodiscard]] bool DoKeepInMemory() const noexcept        { return m_nFlags & eStreamingFlags::STREAMING_KEEP_IN_MEMORY; }
    [[nodiscard]] bool IsPriorityRequest() const noexcept     { return m_nFlags & eStreamingFlags::STREAMING_PRIORITY_REQUEST; }
    [[nodiscard]] bool IsLoadingScene() const noexcept        { return m_nFlags & eStreamingFlags::STREAMING_LOADING_SCENE; }
    [[nodiscard]] bool IsMissionOrGameRequired() const noexcept { return IsMissionRequired() || IsGameRequired(); }
    [[nodiscard]] bool IsRequiredToBeKept() const noexcept      { return IsMissionOrGameRequired() || DoKeepInMemory(); } // GameRequired || MissionRequired || KeepInMemory
};


#endif //LIVERUSSIA_STREAMINGINFO_H
