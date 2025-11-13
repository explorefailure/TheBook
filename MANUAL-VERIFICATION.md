# Plugin Freedom System - Manual Verification Checklist

**Purpose:** Human verification of UX, workflows, and functionality after optimization implementation.

**Automated tests:** Structural/schema validation covered by `./scripts/verify-optimizations.sh`

**This checklist:** User experience, decision quality, workflow behavior, real-world usage.

---

## Pre-Testing Setup

Before starting manual tests:

- [ ] Run automated verification first: `./scripts/verify-optimizations.sh`
- [ ] Ensure all automated tests pass (or document known failures)
- [ ] Have a clean working directory: `git status` shows no uncommitted changes
- [ ] Note current git commit: `git rev-parse --short HEAD`
- [ ] Document testing environment:
  - [ ] macOS version: ________________
  - [ ] JUCE version: ________________
  - [ ] Xcode version: ________________
  - [ ] Date/time: ________________

---

## Test 1: Stage 2+3 Merge (Foundation + Shell Consolidation)

**What changed:** Stage 2 now creates BOTH build system AND parameters in one pass. Old Stage 3 (shell) eliminated.

### Test 1.1: Single Checkpoint Verification

**Steps:**
1. Start fresh plugin: `/dream`
2. Create simple concept (e.g., "Simple gain plugin")
3. Complete creative brief
4. Create parameter spec with 2-3 parameters
5. Run `/plan` to complete Stage 0
6. Run `/implement` to start Stage 2

**Expected behavior:**
- [ ] Stage 2 completes in ONE checkpoint (not two)
- [ ] Output shows: "✓ Build System Ready"
- [ ] NO second checkpoint for "Shell" or "Parameters"
- [ ] Decision menu appears AFTER both build + params complete

**Verify files created:**
- [ ] `CMakeLists.txt` exists at `plugins/[Name]/CMakeLists.txt`
- [ ] `PluginEditor.h/cpp` exist at `plugins/[Name]/Source/`
- [ ] `PluginProcessor.h/cpp` exist with APVTS
- [ ] All parameters from spec implemented in `createParameterLayout()`
- [ ] Parameter IDs match parameter-spec.md exactly

**User experience check:**
- [ ] Checkpoint message is clear (explains what was built)
- [ ] No confusion about "where did Stage 3 go?"
- [ ] Decision menu offers sensible next steps
- [ ] Estimated completion time mentioned
- [ ] No dangling references to "Stage 3" or "shell"

**Edge case:**
- [ ] If build fails, error message is actionable
- [ ] Can retry Stage 2 without state corruption
- [ ] `.continue-here.md` shows correct stage status

**Time checkpoint:** Record Stage 2 completion time: ________ seconds

---

## Test 2: Two-Phase Parameters (Parallel Workflow)

**What changed:** Can now enter draft parameters early, then work on planning and mockups in parallel.

### Test 2.1: Parallel Workflow Happy Path

**Steps:**
1. Start fresh plugin: `/dream`
2. Create concept
3. Complete creative brief
4. When prompted for parameter spec, choose: **"Quick params + parallel workflow"**
5. Enter 3-5 parameters interactively (ID, type, range, default)
6. Verify `parameter-spec-draft.md` created in `.ideas/`
7. Run `/plan` (Stage 0) - should accept draft params
8. Separately run `/mockup` (if available) or create mockup manually
9. After mockup finalized, verify full `parameter-spec.md` generated

**Expected behavior:**
- [ ] Draft parameter entry is quick and interactive
- [ ] `parameter-spec-draft.md` clearly marked as draft
- [ ] Draft file includes warning banner about provisional status
- [ ] Stage 0 proceeds with draft params (doesn't block)
- [ ] UI mockup validates parameter consistency against draft
- [ ] Full `parameter-spec.md` generated after mockup finalized
- [ ] Stage 2 uses full spec (not draft)
- [ ] Draft file preserved (not deleted) for reference

**User experience check:**
- [ ] Parallel workflow feels natural (not confusing)
- [ ] Clear distinction between draft and full spec
- [ ] Decision menus guide user through parallel path
- [ ] No terminology confusion ("draft" vs "full" vs "final")
- [ ] Help text explains when to use parallel workflow

### Test 2.2: Draft Param Validation

**Steps:**
1. Create draft params with 3 parameters
2. Run `/mockup` (or manually create mockup with mismatched params)
3. Intentionally create mockup with 5 controls (mismatch)
4. Run `/sync-design`

**Expected behavior:**
- [ ] Drift detection catches parameter count mismatch
- [ ] Error message explains draft vs mockup discrepancy
- [ ] Suggests resolution (update draft or fix mockup)
- [ ] Specific parameter discrepancies listed
- [ ] Actionable fix commands provided

**Error quality check:**
- [ ] Error message mentions specific parameters missing/extra
- [ ] Suggests which file to edit (draft or mockup)
- [ ] Provides example fix

### Test 2.3: Stage 2 Blocks on Draft Params

**Steps:**
1. Create draft params
2. Skip mockup/full spec generation
3. Try to run `/implement` (start Stage 2)

**Expected behavior:**
- [ ] Stage 2 blocks with clear error
- [ ] Message explains: "Full parameter spec required for implementation"
- [ ] Suggests running mockup workflow to generate full spec
- [ ] Doesn't corrupt workflow state
- [ ] Can resume after generating full spec

**Error message check:**
- [ ] Error is immediate (not discovered mid-Stage 2)
- [ ] Provides clear recovery steps
- [ ] Explains WHY full spec is needed (not just "error")

---

## Test 3: Milestone Language (User-Friendly Terminology)

**What changed:** Replaced "Stage X complete" with benefit-focused milestone names.

### Test 3.1: Completion Messages

**Run full workflow and verify completion messages:**

1. **After Stage 0 (Planning):**
   - [ ] NO "Stage 0 complete" message
   - [ ] Shows user-friendly summary (e.g., "Research and planning complete")
   - [ ] Explains what was created (architecture.md, plan.md)
   - [ ] Milestone name emphasizes benefits, not process

2. **After Stage 2 (Foundation + Shell):**
   - [ ] Shows: "✓ Build System Ready"
   - [ ] Explains: "Your plugin compiles successfully with all parameters implemented"
   - [ ] NO mention of "Stage 2" or "Stage 3"
   - [ ] Describes WHAT WORKS NOW (not what was done)

3. **After Stage 3 (DSP):**
   - [ ] Shows: "✓ Audio Engine Working"
   - [ ] Explains: "Your audio processing is live and ready to test"
   - [ ] NO mention of "Stage 3" or "Stage 4"
   - [ ] Focuses on capability, not completion

4. **After Stage 4 (GUI):**
   - [ ] Shows: "✓ UI Integrated"
   - [ ] Explains: "Your WebView interface is connected to audio engine"
   - [ ] NO mention of "Stage 4" or "Stage 5"
   - [ ] Describes functional UI, not just files created

5. **After Stage 5 (Validation):**
   - [ ] Shows: "✓ Plugin Complete"
   - [ ] Explains: "Passed all validation checks and ready to install"
   - [ ] NO mention of "Stage 5" or "Stage 6"
   - [ ] Emphasizes USABILITY (ready to install/use)

**Terminology consistency check:**
- [ ] No "Stage X complete" anywhere in user output
- [ ] Milestone names consistent across all checkpoints
- [ ] Internal logs/debug can reference stages (that's fine)
- [ ] Documentation uses milestone names

### Test 3.2: Decision Menu Quality

**Verify decision menus use benefit-focused language (not technical jargon):**

**After Stage 2 completion menu:**
- [ ] Option describes benefit: "Implement audio processing - Make your DSP vision a reality"
- [ ] NOT "Continue to Stage 3" or "Run dsp-agent"
- [ ] Option explains WHAT USER GETS (working audio engine)
- [ ] Option is scannable (key info first)

**After Stage 3 completion menu:**
- [ ] Option: "Add GUI - Give your plugin a visual interface"
- [ ] NOT "Continue to Stage 4" or "Run gui-agent"
- [ ] Emphasizes visual/interactive benefit
- [ ] Clear prioritization (recommended option first)

**After Stage 4 completion menu:**
- [ ] Option: "Validate and test - Ensure production readiness"
- [ ] NOT "Continue to Stage 5" or "Run validation-agent"
- [ ] Focuses on quality/reliability benefit
- [ ] Mentions what validation checks

**General menu quality:**
- [ ] Menus describe BENEFITS (what user gets)
- [ ] Menus avoid TECHNICAL stages (Stage numbers, internal terms)
- [ ] Options are scannable and prioritized (best option first)
- [ ] 4-6 options (not overwhelming)
- [ ] "Other" option always present
- [ ] Recommended option clearly marked

### Test 3.3: Help Text and Documentation

**Verify no user-facing docs mention old terminology:**

1. Check decision menus in `.claude/skills/*/assets/decision-menus.json`
2. Check skill descriptions in `.claude/skills/*/SKILL.md` (intro sections)
3. Search for "Stage X complete" in user-facing messages

**Expected:**
- [ ] No "Stage 0 complete" in user messages
- [ ] No "Stage 1 complete" in user messages
- [ ] No "Stage 2 complete" in user messages
- [ ] No "Stage 3 complete" in user messages
- [ ] No "Stage 4 complete" in user messages
- [ ] No "Stage 5 complete" in user messages
- [ ] Milestone names used consistently
- [ ] Internal documentation can still reference stage numbers (that's fine)

**Documentation accuracy:**
- [ ] CLAUDE.md describes 5-stage workflow (not 7)
- [ ] Stage numbers match implementation (0, 2, 3, 4, 5)
- [ ] Milestone language documented in glossary
- [ ] Examples use milestone names

---

## Test 4: Agent Naming Consistency (100% -agent Suffix)

**What changed:** All agents renamed to end with -agent.md (validator→validation-agent, troubleshooter→troubleshoot-agent).

### Test 4.1: Agent Invocation Verification

**Run commands that invoke agents and verify no errors:**

1. `/plan` (invokes research-planning-agent)
   - [ ] No error about missing agent
   - [ ] Agent invoked successfully
   - [ ] Stage 0 completes successfully
   - [ ] Correct agent name in logs

2. `/implement` (invokes foundation-shell-agent, dsp-agent, gui-agent)
   - [ ] Stage 2 invokes foundation-shell-agent (no error)
   - [ ] Stage 3 invokes dsp-agent (no error)
   - [ ] Stage 4 invokes gui-agent (no error)
   - [ ] Agent names consistent with -agent suffix

3. `/test` (invokes validation-agent)
   - [ ] No error about "validator not found"
   - [ ] Validation runs successfully
   - [ ] Agent name shows as "validation-agent"

4. `/research` (invokes troubleshoot-agent)
   - [ ] No error about "troubleshooter not found"
   - [ ] Research runs successfully
   - [ ] Agent name shows as "troubleshoot-agent"

### Test 4.2: Error Message Quality

**If agent invocation fails, error message should be helpful:**

Manually trigger agent error (e.g., rename agent file):
- [ ] Error mentions the CORRECT agent name (with -agent suffix)
- [ ] Doesn't reference old agent names (validator, troubleshooter)
- [ ] Suggests actionable fix (e.g., "Run /setup to verify installation")
- [ ] Shows expected path: `.claude/agents/[name]-agent.md`

**Consistency check:**
- [ ] All agent references use -agent suffix
- [ ] No mixed terminology (some with suffix, some without)
- [ ] File system matches naming (ls .claude/agents/)

---

## Test 5: Stage 0+1 Merge (Research + Planning Consolidation)

**What changed:** Stage 0 now creates BOTH architecture.md AND plan.md in one pass. Old Stage 1 eliminated.

### Test 5.1: Single Checkpoint Verification

**Steps:**
1. Create fresh plugin with `/dream`
2. Complete creative brief and parameter spec
3. Run `/plan`

**Expected behavior:**
- [ ] Stage 0 completes in ONE checkpoint (not two)
- [ ] BOTH `architecture.md` AND `plan.md` created in `.ideas/`
- [ ] NO second checkpoint for "Planning" separate from "Research"
- [ ] Decision menu prompts: "Start implementation - Begin building your plugin"
- [ ] Completion message uses milestone language (not "Stage 0+1 complete")

**Verify content quality:**
- [ ] `architecture.md` contains DSP design (not placeholder)
- [ ] `plan.md` contains implementation strategy (not placeholder)
- [ ] Both files reference each other consistently (no contradictions)
- [ ] JUCE API research is thorough
- [ ] Professional plugin comparisons included
- [ ] Implementation milestones clearly defined

**File structure check:**
- [ ] Files in correct location: `plugins/[Name]/.ideas/`
- [ ] File naming consistent (architecture.md, plan.md)
- [ ] Both files have YAML frontmatter
- [ ] Cross-references valid (no broken links)

**User experience check:**
- [ ] Workflow feels streamlined (not confusing)
- [ ] No confusion about "where did Stage 1 go?"
- [ ] Clear transition from Stage 0 → Stage 2
- [ ] Single decision point (not two separate menus)

### Test 5.2: research-planning-agent Content Quality

**Verify the merged agent produces quality output:**

**Architecture.md quality:**
- [ ] DSP design has sufficient technical depth
- [ ] JUCE API classes identified correctly
- [ ] Signal flow diagram/description included
- [ ] Complexity analysis present
- [ ] Professional plugin references relevant

**Plan.md quality:**
- [ ] Implementation strategy is actionable
- [ ] Milestones clearly defined
- [ ] Dependencies identified
- [ ] Risk assessment included
- [ ] Success criteria defined

**Integration quality:**
- [ ] Architecture.md and plan.md are coherent (not contradictory)
- [ ] Plan references architecture correctly
- [ ] No duplication between files
- [ ] Clear separation of concerns (design vs strategy)

**Compare to old workflow (if you have historical plugins):**
- [ ] New Stage 0 output is AS GOOD OR BETTER than old Stage 0+1 combined
- [ ] No loss of quality from consolidation
- [ ] Research depth maintained
- [ ] Planning detail maintained

**Time checkpoint:** Record Stage 0 completion time: ________ seconds

---

## Test 6: Validation Caching System

**What changed:** design-sync now caches validation results to avoid redundant checks.

### Test 6.1: Cache Miss (First Run)

**Steps:**
1. Create plugin with mockup: `/dream` → creative brief → `/mockup`
2. Run `/sync-design MyPlugin`
3. Observe output and timing
4. Note start and end time

**Expected behavior:**
- [ ] First run shows: "Running design-sync validation..."
- [ ] Full validation executes (takes several seconds)
- [ ] Output shows validation results (pass/fail per check)
- [ ] Cache file created: `.claude/cache/validation-results.json`
- [ ] Cache file has correct structure (plugin name, checksums, expiry)

**Cache file verification:**
- [ ] File exists at `.claude/cache/validation-results.json`
- [ ] JSON is valid (can parse with `jq`)
- [ ] Contains entry for MyPlugin
- [ ] Has creative_brief_checksum and mockup_checksum
- [ ] Has expires_at timestamp (15 minutes from now)
- [ ] Has validation_result (pass/fail/partial)

**Time checkpoint:** First run duration: ________ seconds

### Test 6.2: Cache Hit (No Changes)

**Steps:**
1. Immediately run `/sync-design MyPlugin` again (no changes to files)
2. Observe output and timing

**Expected behavior:**
- [ ] Shows: "✓ design-sync cache hit (content unchanged)"
- [ ] Completes INSTANTLY (< 1 second)
- [ ] Skips full validation (uses cached result)
- [ ] Output shows cached validation status
- [ ] Mentions cache expiry time

**Performance check:**
- [ ] Cache hit is noticeably faster than cache miss (>10x)
- [ ] User doesn't wait for redundant validation
- [ ] Response feels immediate

**Time checkpoint:** Cache hit duration: ________ seconds
**Speedup factor:** ________ x faster

### Test 6.3: Cache Invalidation (Content Changed)

**Steps:**
1. Edit `creative-brief.md`: Add a new section or modify existing content
2. Run `/sync-design MyPlugin`
3. Observe output

**Expected behavior:**
- [ ] Shows: "Cache miss (content changed)"
- [ ] Full validation runs again
- [ ] New checksum stored in cache
- [ ] Validation reflects updated content
- [ ] Cache updated with new checksums

**Verification:**
- [ ] Cache file updated (check timestamp)
- [ ] New checksums differ from previous
- [ ] Validation results reflect changes

**Test mockup change:**
1. Edit mockup file (change UI element)
2. Run `/sync-design MyPlugin`

**Expected:**
- [ ] Cache miss (mockup checksum changed)
- [ ] Full validation runs
- [ ] Detects mockup changes

### Test 6.4: Cache Expiry

**Steps:**
1. Run `/sync-design MyPlugin` (create cache)
2. Manually edit `.claude/cache/validation-results.json`
3. Change `expires_at` timestamp to past date (e.g., 2020-01-01)
4. Run `/sync-design MyPlugin`

**Expected behavior:**
- [ ] Shows: "Cache miss (expired)"
- [ ] Full validation runs
- [ ] Fresh cache entry created with new expiry (15 min from now)

**Time check:**
- [ ] Default expiry is 15 minutes from creation
- [ ] Expiry format is ISO 8601 (e.g., "2025-11-13T14:30:00Z")

### Test 6.5: Manual Cache Clearing

**Steps:**
1. Run `/clear-cache MyPlugin` (single plugin)
2. Verify cache cleared for MyPlugin only
3. Run `/sync-design MyPlugin`

**Expected behavior:**
- [ ] `/clear-cache` confirms cache cleared for MyPlugin
- [ ] Next `/sync-design` shows cache miss
- [ ] Full validation runs
- [ ] Other plugins' cache entries preserved

**Test system-wide clear:**
1. Run `/clear-cache --all` (or equivalent)
2. Verify entire cache file cleared/deleted

**Expected:**
- [ ] All cache entries removed
- [ ] Confirmation message shown
- [ ] Next `/sync-design` on ANY plugin shows cache miss

### Test 6.6: Cache Isolation (Multiple Plugins)

**Steps:**
1. Create two plugins: PluginA and PluginB
2. Run `/sync-design PluginA` (creates cache entry)
3. Run `/sync-design PluginB` (creates separate cache entry)
4. Modify PluginA creative-brief.md
5. Run `/sync-design PluginA`
6. Run `/sync-design PluginB` (unchanged)

**Expected behavior:**
- [ ] PluginA cache invalidated (content changed)
- [ ] PluginB cache still valid (unchanged)
- [ ] Cache isolation prevents cross-plugin contamination
- [ ] Each plugin has separate cache entry

**Cache structure check:**
- [ ] `.claude/cache/validation-results.json` has object with plugin keys
- [ ] Each plugin entry is independent
- [ ] Checksums are plugin-specific

---

## Test 7: End-to-End Workflow Integration

**Purpose:** Verify all optimizations work together seamlessly in a complete plugin development workflow.

### Test 7.1: Full Workflow (New Plugin from Scratch)

**Steps:**
1. `/dream` - Create new plugin concept
2. Complete creative brief (describe a simple plugin, e.g., "3-band EQ")
3. Choose **parallel workflow** (draft params)
4. Enter 3-5 parameters interactively
5. `/plan` - Run Stage 0 (research + planning)
6. Verify BOTH architecture.md and plan.md created
7. Create UI mockup (via `/mockup` or manually)
8. Verify full parameter-spec.md generated
9. `/sync-design MyPlugin` - Validate design consistency
10. `/implement` - Start Stage 2 (foundation + shell)
11. Verify build system + parameters created together
12. Continue through Stage 3 (DSP)
13. Continue through Stage 4 (GUI)
14. `/test` - Run validation
15. `/install-plugin MyPlugin` - Install to system

**Expected behavior:**
- [ ] Entire workflow completes without errors
- [ ] Stage 0: Single checkpoint (research + planning together)
- [ ] Stage 2: Single checkpoint (foundation + shell together)
- [ ] Milestone language throughout (no "Stage X complete")
- [ ] Agent invocations succeed (all -agent names work)
- [ ] Validation caching works (second `/sync-design` instant)
- [ ] Plugin builds, validates, and installs successfully

**Checkpoint counting:**
- [ ] Total checkpoints: 5 (Stage 0, 2, 3, 4, 5)
- [ ] NOT 7 checkpoints (old workflow)
- [ ] Each checkpoint clear and purposeful

**User experience check:**
- [ ] Workflow feels streamlined (not fragmented)
- [ ] Fewer checkpoints = less friction
- [ ] Decision menus guide user naturally
- [ ] No confusing terminology or stage numbers
- [ ] Progress indicators clear
- [ ] Estimated time accurate

**Time checkpoint:** Total workflow duration: ________ minutes

### Test 7.2: Resume Workflow (Continue Paused Plugin)

**Steps:**
1. Start plugin workflow, complete Stage 2
2. Exit Claude Code session (or just wait)
3. Return and run `/continue MyPlugin`

**Expected behavior:**
- [ ] Resume detects correct stage (Stage 2 complete)
- [ ] Shows current status summary
- [ ] Prompts for next action (Stage 3 - DSP)
- [ ] Workflow continues seamlessly
- [ ] State consistency maintained
- [ ] No loss of context

**State file verification:**
- [ ] `.continue-here.md` has correct plugin name
- [ ] Current stage accurate
- [ ] Last checkpoint accurate
- [ ] Next action suggested

**User experience:**
- [ ] Resume is intuitive (user doesn't need to remember where they were)
- [ ] Clear context about what's done and what's next
- [ ] No confusion about stage numbering

### Test 7.3: Improve Existing Plugin

**Steps:**
1. Complete plugin from Test 7.1
2. Run `/improve MyPlugin "Add new parameter"`
3. Verify regression testing runs
4. Verify backup created
5. Verify versioning works
6. Rebuild and test

**Expected behavior:**
- [ ] Improvement workflow uses correct agent names
- [ ] No errors from agent renames
- [ ] Regression tests pass
- [ ] Plugin still works after improvement
- [ ] Backup created before changes
- [ ] Version bumped correctly

**Backup verification:**
- [ ] Backup exists at `plugins/MyPlugin-backup-v1.0.0/`
- [ ] Backup contains all source files
- [ ] Can restore from backup if needed

**Testing:**
- [ ] Regression tests run automatically
- [ ] New feature works
- [ ] Old features still work

---

## Test 8: Error Handling and Edge Cases

**Purpose:** Verify graceful failure and helpful error messages.

### Test 8.1: Invalid State Transitions

**Steps:**
1. Create plugin, complete Stage 0
2. Try to skip to Stage 4 (e.g., manually edit `.continue-here.md` to say "stage: 4")
3. Run `/continue MyPlugin`

**Expected behavior:**
- [ ] Error message explains preconditions not met
- [ ] Mentions Stage 2 and 3 must complete first
- [ ] Suggests correct action (run Stage 2)
- [ ] Doesn't corrupt workflow state
- [ ] `.continue-here.md` not changed incorrectly

**Error quality:**
- [ ] Error is clear and specific
- [ ] Explains WHY state transition invalid
- [ ] Provides recovery steps

### Test 8.2: Missing Contract Files

**Steps:**
1. Create plugin, complete Stage 0
2. Manually delete `architecture.md`
3. Try to run `/implement`

**Expected behavior:**
- [ ] Error explains missing contract file
- [ ] Mentions which file is missing (architecture.md)
- [ ] Suggests regenerating with `/plan`
- [ ] Doesn't proceed with corrupted state
- [ ] Can recover by re-running `/plan`

**Test multiple missing files:**
1. Delete both `architecture.md` and `plan.md`
2. Try to run `/implement`

**Expected:**
- [ ] Error lists all missing files
- [ ] Suggests comprehensive fix
- [ ] Clear recovery path

### Test 8.3: Build Failures

**Steps:**
1. Create plugin through Stage 2
2. Manually introduce build error in `PluginProcessor.cpp` (e.g., syntax error)
3. Run Stage 3 (DSP)
4. Build fails

**Expected behavior:**
- [ ] Error message shows build output
- [ ] Compiler error is visible
- [ ] Suggests troubleshooting steps
- [ ] Can retry after fixing code
- [ ] State doesn't advance to Stage 4 on failure
- [ ] `.continue-here.md` stays at Stage 3

**Error output quality:**
- [ ] Build errors are readable (not buried in output)
- [ ] File and line number shown
- [ ] Error highlighted or formatted
- [ ] Suggests common fixes (if applicable)

**Recovery:**
- [ ] Fix syntax error
- [ ] Re-run Stage 3
- [ ] Build succeeds
- [ ] Workflow continues

### Test 8.4: Validation Failures

**Steps:**
1. Complete plugin through Stage 4
2. Intentionally introduce pluginval failure:
   - Remove `prepareToPlay()` method from PluginProcessor.cpp
   - Or introduce audio processing bug
3. Run `/test`

**Expected behavior:**
- [ ] Validation catches error
- [ ] Error message explains failure (which test failed)
- [ ] Shows pluginval output
- [ ] Suggests fixes
- [ ] Doesn't mark Stage 5 complete on failure
- [ ] Can retry after fixing

**Error quality:**
- [ ] Specific test failure identified
- [ ] Explanation of what the error means
- [ ] Actionable fix suggestions
- [ ] Links to docs if applicable

### Test 8.5: Parallel Workflow Edge Cases

**Test 8.5a: Draft params without mockup**
1. Create draft params
2. Skip mockup
3. Try to generate full parameter-spec.md

**Expected:**
- [ ] Error: "Mockup required to finalize parameters"
- [ ] Suggests creating mockup first

**Test 8.5b: Mockup without params**
1. Skip draft params
2. Create mockup
3. System should handle gracefully

**Expected:**
- [ ] Mockup creation works
- [ ] Parameter extraction from mockup (if supported)
- [ ] OR: Prompts for parameter spec after mockup

---

## Test 9: Performance and Timing

**Purpose:** Verify optimizations actually improve speed and reduce friction.

### Test 9.1: Checkpoint Reduction

**Measure checkpoint count for full workflow:**

**Old workflow (before optimizations):**
- Stage 0 (research): 1 checkpoint
- Stage 1 (planning): 1 checkpoint
- Stage 2 (foundation): 1 checkpoint
- Stage 3 (shell): 1 checkpoint
- Stage 4 (DSP): 1 checkpoint
- Stage 5 (GUI): 1 checkpoint
- Stage 6 (validation): 1 checkpoint
- **Total: 7 checkpoints**

**New workflow (after optimizations):**
- Stage 0 (research + planning): 1 checkpoint
- Stage 2 (foundation + shell): 1 checkpoint
- Stage 3 (DSP): 1 checkpoint
- Stage 4 (GUI): 1 checkpoint
- Stage 5 (validation): 1 checkpoint
- **Total: 5 checkpoints**

**Verify:**
- [ ] New workflow has EXACTLY 5 checkpoints (not 7)
- [ ] 2 fewer decision points = smoother experience
- [ ] Total time reduced (fewer context switches)
- [ ] User doesn't lose focus from excessive menu prompts

**User experience comparison:**
- [ ] Workflow feels faster (subjective)
- [ ] Less "start-stop" friction
- [ ] More continuous progress

### Test 9.2: Validation Caching Speedup

**Measure time for repeated `/sync-design` calls:**

**First run (cache miss):**
- [ ] Start time: ________
- [ ] End time: ________
- [ ] Duration: ________ seconds

**Second run (cache hit, no changes):**
- [ ] Start time: ________
- [ ] End time: ________
- [ ] Duration: ________ seconds

**Speedup calculation:**
- [ ] Speedup factor: ________ x faster
- [ ] Verify: >10x speedup (e.g., 8s → 0.5s)

**User perception:**
- [ ] Second run is noticeably faster
- [ ] User doesn't wait for redundant validation
- [ ] Performance improvement is obvious

### Test 9.3: Stage Merge Timing

**Measure time for merged stages vs estimated old timing:**

**Stage 0+1 merge:**
- [ ] New Stage 0 duration: ________ seconds
- [ ] Estimated old Stage 0: ________ seconds
- [ ] Estimated old Stage 1: ________ seconds
- [ ] Total old time: ________ seconds
- [ ] Time saved: ________ seconds (or overhead if negative)

**Stage 2+3 merge:**
- [ ] New Stage 2 duration: ________ seconds
- [ ] Estimated old Stage 2: ________ seconds
- [ ] Estimated old Stage 3: ________ seconds
- [ ] Total old time: ________ seconds
- [ ] Time saved: ________ seconds

**Analysis:**
- [ ] Merges save time (less overhead) OR
- [ ] Merges have minimal time impact but reduce friction

---

## Test 10: Backward Compatibility

**Purpose:** Ensure existing plugins and workflows still work.

### Test 10.1: Existing Plugin Compatibility

**If you have plugins created BEFORE optimizations:**

1. List existing plugins: `cat PLUGINS.md`
2. Pick a plugin created before commit 229973cf
3. Run `/continue OldPlugin`

**Expected behavior:**
- [ ] Resume detects correct stage (even if old stage numbering)
- [ ] Workflow continues correctly
- [ ] No errors from agent renames
- [ ] Decision menus use new terminology
- [ ] State migration handled gracefully

**Test improvement on old plugin:**
1. Run `/improve OldPlugin "Minor fix"`
2. Verify improvement workflow works

**Expected:**
- [ ] Agent invocations work (new names)
- [ ] Regression tests run
- [ ] Plugin still functional

### Test 10.2: Old `.continue-here.md` Files

**If you have old continue files with old stage references:**

1. Check `.continue-here.md` for stage references
2. Look for "stage: 1" or "stage: 3" (eliminated stages)
3. Try to resume workflow

**Expected behavior:**
- [ ] System handles old stage numbers gracefully
- [ ] Migrates to new stage numbering if needed
- [ ] Doesn't break on old terminology
- [ ] User sees updated stage names (not old numbers)

**Migration verification:**
- [ ] Old "stage: 1" → maps to "stage: 0" (planning part)
- [ ] Old "stage: 3" → maps to "stage: 2" (shell part)
- [ ] Checkpoint logic adjusted correctly

### Test 10.3: Legacy Parameter Files

**If you have plugins with old parameter-spec.md format:**

1. Find plugin with old parameter spec
2. Run `/sync-design OldPlugin`
3. Try to improve plugin

**Expected:**
- [ ] Old parameter format still readable
- [ ] Validation works (or gracefully fails with upgrade suggestion)
- [ ] Can migrate to new format if needed

---

## Test 11: Decision Menu and UX Polish

**Purpose:** Verify decision menus are high-quality and user-friendly.

### Test 11.1: Menu Structure and Clarity

**Check decision menus at each checkpoint:**

**After Stage 0:**
- [ ] Menu has 4-6 options (not overwhelming)
- [ ] Primary action clearly recommended (marked or first)
- [ ] Options describe benefits, not technical tasks
- [ ] "Other" option present for custom input
- [ ] Options are mutually exclusive (no overlap)

**After Stage 2:**
- [ ] Primary: "Continue to DSP" (or similar benefit-focused)
- [ ] Secondary: "Test build" or "Review code"
- [ ] Discovery: "Learn about audio processing" (optional)
- [ ] Always: "Other"

**After Stage 3:**
- [ ] Primary: "Add GUI"
- [ ] Secondary: "Test audio processing"
- [ ] Options prioritized by typical workflow

**General menu quality:**
- [ ] No jargon (APVTS, pluginval, etc.) without explanation
- [ ] Scannable (key info first, details after)
- [ ] Consistent formatting across all menus
- [ ] No typos or grammar errors

### Test 11.2: Error Message Quality

**Trigger various errors and verify message quality:**

**Build error:**
- [ ] Error type identified (syntax, linker, CMake, etc.)
- [ ] Specific file and line number (if applicable)
- [ ] Explanation of what went wrong
- [ ] Suggested fix or troubleshooting steps
- [ ] Link to docs or similar issues (if applicable)

**Validation error:**
- [ ] Test name that failed
- [ ] Explanation of test purpose
- [ ] What specifically failed
- [ ] How to fix it
- [ ] Example code (if applicable)

**State error:**
- [ ] Clear description of state inconsistency
- [ ] What was expected vs what was found
- [ ] Recovery steps
- [ ] Can reset state if needed

**General error quality:**
- [ ] No raw stack traces (unless debug mode)
- [ ] Human-readable messages
- [ ] Actionable steps (not just "error occurred")
- [ ] Appropriate urgency (critical vs warning)

### Test 11.3: Progress Indicators

**Verify progress indicators are clear:**

**During long operations:**
- [ ] Spinner or progress message shown
- [ ] User knows something is happening (not frozen)
- [ ] Estimated time mentioned (if available)
- [ ] Can cancel operation (if applicable)

**Between stages:**
- [ ] Clear indication of current stage
- [ ] Progress bar or stage list (e.g., "Stage 2 of 5")
- [ ] What's been completed
- [ ] What's coming next

**After completion:**
- [ ] Success confirmation clear
- [ ] Summary of what was accomplished
- [ ] Next steps obvious

---

## Sign-Off Checklist

After completing all tests, verify:

### Optimizations Verified

- [ ] **Stage 2+3 merge (Test 1):**
  - [ ] Single checkpoint creates build + params
  - [ ] No Stage 3 references in user output
  - [ ] Files created correctly

- [ ] **Two-phase parameters (Test 2):**
  - [ ] Draft params enable parallel workflow
  - [ ] Full spec generated after mockup
  - [ ] Stage 2 blocks on draft params

- [ ] **Milestone language (Test 3):**
  - [ ] No "Stage X complete" in user messages
  - [ ] Benefit-focused completion messages
  - [ ] Decision menus use milestone language

- [ ] **Agent naming (Test 4):**
  - [ ] 100% consistent -agent suffix
  - [ ] All agent invocations succeed
  - [ ] Error messages use correct names

- [ ] **Stage 0+1 merge (Test 5):**
  - [ ] Single checkpoint creates architecture + plan
  - [ ] Content quality maintained
  - [ ] Clear transition to Stage 2

- [ ] **Validation caching (Test 6):**
  - [ ] Cache hits skip validation
  - [ ] >10x speedup on cache hit
  - [ ] Cache invalidation works correctly

### User Experience Verified

- [ ] **Workflow streamlined:**
  - [ ] 5 checkpoints total (not 7)
  - [ ] Fewer interruptions
  - [ ] Smoother progress

- [ ] **Terminology consistent:**
  - [ ] Milestone language throughout
  - [ ] No technical jargon without explanation
  - [ ] User-friendly messages

- [ ] **Performance improved:**
  - [ ] Validation caching observable
  - [ ] Stage merges reduce friction
  - [ ] Total time reduced (or equal with less friction)

- [ ] **Error handling robust:**
  - [ ] Helpful error messages
  - [ ] Clear recovery steps
  - [ ] No state corruption

### Quality Maintained

- [ ] **Functionality preserved:**
  - [ ] All features work
  - [ ] No regressions
  - [ ] Backward compatibility maintained

- [ ] **Content quality:**
  - [ ] Stage 0 output quality unchanged
  - [ ] Stage 2 output quality unchanged
  - [ ] No loss of depth or detail

- [ ] **Documentation accurate:**
  - [ ] CLAUDE.md reflects changes
  - [ ] Decision menus updated
  - [ ] Help text correct

### No Critical Issues

- [ ] **No show-stoppers:**
  - [ ] All workflows complete successfully
  - [ ] No data corruption
  - [ ] No broken features
  - [ ] No security issues

- [ ] **Edge cases handled:**
  - [ ] Invalid state transitions caught
  - [ ] Missing files detected
  - [ ] Build failures graceful
  - [ ] Validation failures actionable

---

## Notes and Observations

**Use this section to record:**

### Positive Observations
- Performance improvements observed:
  -
- UX improvements noticed:
  -
- Features that work particularly well:
  -

### Issues Discovered
- Bugs or unexpected behavior:
  -
- Confusing UX or messaging:
  -
- Performance regressions:
  -

### Suggestions for Further Optimization
- Additional improvements to consider:
  -
- Features to add:
  -
- Documentation to clarify:
  -

### Test Environment Notes
- System configuration:
  -
- Plugins tested:
  -
- Total testing time: ________ hours

---

## Rollback Criteria

**If any of these are true, consider rolling back optimizations:**

### Critical Failures
- [ ] **Broken workflows:** Can't complete plugin development end-to-end
- [ ] **Data corruption:** State files corrupted, plugins broken
- [ ] **Agent failures:** Multiple agents fail to invoke
- [ ] **Build system broken:** Can't compile plugins

### Major Regressions
- [ ] **Significant performance regression:** >20% slower overall
- [ ] **Quality degradation:** Stage 0 or Stage 2 output significantly worse
- [ ] **Multiple critical bugs:** >3 critical issues discovered
- [ ] **Backward compatibility broken:** Existing plugins don't work

### User Experience Issues
- [ ] **Increased confusion:** New terminology/flow is MORE confusing than before
- [ ] **Loss of features:** Functionality removed without replacement
- [ ] **Error handling worse:** Errors are less actionable than before

**Rollback procedure:**
```bash
# Show commits since optimization implementation
git log --oneline 229973cf..HEAD

# Option 1: Revert specific commit
git revert <commit-hash>

# Option 2: Reset to before optimizations (DESTRUCTIVE - loses all work after 229973cf)
git reset --hard 229973cf

# Option 3: Create rollback branch (preserves work)
git checkout -b rollback-optimizations
git reset --hard 229973cf
```

**Before rollback:**
- [ ] Document all issues discovered
- [ ] Save test results and observations
- [ ] Notify team/users of rollback
- [ ] Plan fixes for issues before re-deploying

---

## Success Criteria

**Manual verification is complete when:**

### All Tests Complete
- ✓ Test 1: Stage 2+3 merge verified
- ✓ Test 2: Two-phase parameters verified
- ✓ Test 3: Milestone language verified
- ✓ Test 4: Agent naming verified
- ✓ Test 5: Stage 0+1 merge verified
- ✓ Test 6: Validation caching verified
- ✓ Test 7: End-to-end workflow verified
- ✓ Test 8: Error handling verified
- ✓ Test 9: Performance verified
- ✓ Test 10: Backward compatibility verified
- ✓ Test 11: UX polish verified

### Quality Gates Met
- ✓ All 6 optimizations working correctly
- ✓ No critical regressions found
- ✓ User experience equal or better than before
- ✓ Performance improvements observable
- ✓ Documentation accurate and helpful
- ✓ Backward compatibility maintained

### Verdict
**Overall assessment:** [ PASS / FAIL / CONDITIONAL PASS ]

**Conditional pass criteria:** Minor issues discovered but not critical, with remediation plan defined.

**Recommendations:**
- [ ] Deploy to production
- [ ] Fix minor issues then deploy
- [ ] Significant changes needed before deployment
- [ ] Rollback recommended

---

## Tester Sign-Off

**Tester name:** ________________________________

**Date:** ______________ **Time:** ______________

**Testing duration:** ________ hours

**Plugins tested:** ________________________________

**Git commit tested:** ________________________________

**Verdict:** [ PASS / FAIL / CONDITIONAL PASS ]

**Additional notes:**
_______________________________________________________________
_______________________________________________________________
_______________________________________________________________

**Signature:** ________________________________

---

## Appendix: Quick Reference

### Test Summary Checklist
- [ ] Pre-testing setup complete
- [ ] Test 1: Stage 2+3 merge
- [ ] Test 2: Two-phase parameters
- [ ] Test 3: Milestone language
- [ ] Test 4: Agent naming
- [ ] Test 5: Stage 0+1 merge
- [ ] Test 6: Validation caching
- [ ] Test 7: End-to-end workflow
- [ ] Test 8: Error handling
- [ ] Test 9: Performance
- [ ] Test 10: Backward compatibility
- [ ] Test 11: Decision menu UX
- [ ] Sign-off complete

### Key File Locations
- Automated tests: `./scripts/verify-optimizations.sh`
- Validation cache: `.claude/cache/validation-results.json`
- Plugin state: `.continue-here.md`
- Plugin registry: `PLUGINS.md`
- Agent files: `.claude/agents/*-agent.md`
- Skill files: `.claude/skills/*/SKILL.md`

### Key Commands
- `/dream` - Start new plugin
- `/plan` - Run Stage 0 (research + planning)
- `/implement` - Start implementation
- `/continue [Name]` - Resume workflow
- `/sync-design [Name]` - Validate design
- `/test [Name]` - Run validation
- `/improve [Name]` - Modify plugin
- `/clear-cache [Name]` - Clear validation cache

### Expected Stage Count
- **Total stages:** 5 (0, 2, 3, 4, 5)
- **Checkpoints:** 5 (one per stage)
- **Old workflow:** 7 stages (0, 1, 2, 3, 4, 5, 6)
- **Reduction:** 2 fewer checkpoints

### Performance Benchmarks
- Validation cache hit: <1 second
- Validation cache miss: 5-10 seconds
- Speedup factor: >10x
- Stage merge overhead: Minimal or negative (time saved)

---

**End of Manual Verification Checklist**
