# Title Page Skill

> **Session landing page showing projects and all available commands.**

## Trigger Conditions
- User runs `/titlepage` command
- User says "show title page", "home", "dashboard", "what can I do"
- Beginning of session orientation

---

## Process

### 1. Display Banner

```
████████╗██╗  ██╗███████╗    ██████╗  ██████╗  ██████╗ ██╗  ██╗
╚══██╔══╝██║  ██║██╔════╝    ██╔══██╗██╔═══██╗██╔═══██╗██║ ██╔╝
   ██║   ███████║█████╗      ██████╔╝██║   ██║██║   ██║█████╔╝
   ██║   ██╔══██║██╔══╝      ██╔══██╗██║   ██║██║   ██║██╔═██╗
   ██║   ██║  ██║███████╗    ██████╔╝╚██████╔╝╚██████╔╝██║  ██╗
   ╚═╝   ╚═╝  ╚═╝╚══════╝    ╚═════╝  ╚═════╝  ╚═════╝ ╚═╝  ╚═╝

         - A CHOOSE YOUR OWN AU/VST PLUGIN ADVENTURE -
```

### 2. Read Projects from PLUGINS.md

Read `/Users/grot/thebook/PLUGINS.md` and extract the plugin table.

Display active projects (exclude 🗑️ Archived):
```
PROJECTS
├── PluginName     💡 Ideated
├── PluginName     🚧 Stage N
└── PluginName     📦 Installed (vX.X.X)
```

### 3. Display All Commands

Organize commands by category:

**Create & Plan**
| Command | Description |
|---------|-------------|
| `/dream [Name]` | Brainstorm new plugin |
| `/plan [Name]` | Research & architecture |

**Build**
| Command | Description |
|---------|-------------|
| `/implement [Name]` | Build stages 1-3 |
| `/continue [Name]` | Resume from checkpoint |

**Test & Validate**
| Command | Description |
|---------|-------------|
| `/test [Name]` | Run validation |
| `/research [topic]` | Deep investigation |

**Deploy**
| Command | Description |
|---------|-------------|
| `/install-plugin [Name]` | Install to system |
| `/package [Name]` | Create PKG installer |

**Modify**
| Command | Description |
|---------|-------------|
| `/improve [Name] [desc]` | Versioned changes |
| `/doc-fix [context]` | Document solved problem |
| `/add-critical-pattern [name]` | Add to Required Reading |

**Cleanup**
| Command | Description |
|---------|-------------|
| `/uninstall [Name]` | Remove binaries |
| `/reset-to-ideation [Name]` | Keep ideas, remove code |
| `/destroy [Name]` | Complete removal |
| `/clean [Name]` | Interactive menu |
| `/clear-cache [Name]` | Clear validation cache |

**System**
| Command | Description |
|---------|-------------|
| `/setup` | Validate dependencies |
| `/reconcile [Name]` | Repair state files |
| `/thebook` | Reference guide |
| `/pfs` | Load system context |
| `/show-standalone [Name]` | Open plugin UI |
| `/titlepage` | This screen |

---

## Output Format

Use box-drawing characters for clean terminal display:
- `━` for horizontal rules
- `├──` and `└──` for tree structure
- Consistent column alignment for commands

---

## Success Criteria
- Banner displays correctly
- All non-archived projects listed with current status
- All commands shown with descriptions
- Clean, scannable layout
