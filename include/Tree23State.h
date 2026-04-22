#pragma once
#include "VisualizerState.h"
#include "Tree23.h"
#include "Tree23Node.h"
#include "Tree23Step.h"
#include "TreeSnapshot.h"
#include "VisualizerUI.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "CodeBox.h"

class Tree23State : public VisualizerState
{
public:
    Tree23State(App* app);
    ~Tree23State() = default;

    void init() override;
    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    Tree23 m_tree;
    VisualizerUI m_ui;
    CodeBox m_codeBox;

    std::vector<Tree23Node*> m_highlightPath;
    std::vector<Tree23Node*> m_fullSearchPath;
    std::vector<Tree23Node*> m_insertPath;
    Tree23Node* m_currentHighlightedNode = nullptr;

    std::vector<Tree23Step> m_insertSteps;
    std::vector<Tree23Step> m_deleteSteps;

    bool m_searchFound = false;
    bool m_isPlaying = false;
    int m_currentAnimationStep = -1;

    bool m_isInsertPlaying = false;
    int m_currentInsertStep = -1;
    int m_pendingInsertValue = 0;

    bool m_isDeletePlaying = false;
    int m_currentDeleteStep = -1;
    int m_pendingDeleteValue = 0;

    bool m_isUpdatePlaying = false;
    int m_pendingOldValue = 0;
    int m_pendingNewValue = 0;

    TreeSnapshot m_currentSnapshot;
    TreeSnapshot m_snapshotFrom;
    TreeSnapshot m_snapshotTo;

    bool m_isSnapshotTransitionPlaying = false;
    float m_snapshotTransitionProgress = 0.f;
    float m_snapshotTransitionDuration = 0.45f;

    SnapshotSequence m_currentSequence;
    int m_currentSequenceIndex = 0;

    SnapshotSequence m_currentSearchSequence;
    bool m_isSearchSequencePlaying = false;
    int m_currentSearchSequenceIndex = 0;

    sf::Clock m_animationClock;
    sf::Clock m_snapshotTransitionClock;
    float m_animationDelay = 0.8f;
    std::string m_speedLabel = "Normal";

    void resetCodeBox();
    void showInsertCode(int step);
    void showSearchCode(int step, bool found = false);
    void showDeleteCode(int step);
    void showUpdateCode(int step);
};