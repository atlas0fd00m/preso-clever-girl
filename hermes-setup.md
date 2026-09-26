# Set Up Ollama Cloud + Hermes
**⏱ 5–10 min · Goal: Get a free cloud LLM running with Hermes Agent — no GPU required**

This exercise gets you from zero to a working AI agent with no paid accounts and no powerful hardware. You'll install the Ollama client, sign in to Ollama's free cloud tier, configure Hermes to use cloud models, and verify everything works.

> **Why cloud instead of local?** Ollama's cloud models run on Ollama's GPUs, not yours. You get access to large, capable models without needing a gaming GPU or tons of RAM. The free tier includes "light usage" — more than enough for these exercises. Session limits reset every 5 hours; weekly limits reset every 7 days.

---

## Step 1: Install the Ollama Client

You need the Ollama client to sign in and manage cloud models, but you won't be downloading model weights locally.

**Linux:**
```bash
curl -fsSL https://ollama.com/install.sh | sh
```

**macOS:**
```bash
# Option A: Download the app → https://ollama.com/download
# Option B: Homebrew
brew install ollama
```

**Windows:**
```
# Download the installer from https://ollama.com/download
# Run the .exe, follow the wizard
```

**Verify:**
```bash
ollama --version
```

---

## Step 2: Create an Ollama Account and Sign In

Ollama's cloud models require a free account on ollama.com.

**Sign in or create an account from the CLI:**
```bash
ollama signin
```

This opens a browser for OAuth authentication. Once signed in, your credentials are stored locally and the Ollama client can access cloud models.

**Quick test — run a cloud model directly:**
```bash
ollama run gpt-oss:20b-cloud "What is a stack buffer overflow? One paragraph."
```

If you get a reasonable answer, cloud access works. Type `/bye` to exit an interactive session.

> **Note:** Cloud model names use a `-cloud` suffix in the CLI (e.g. `gpt-oss:20b-cloud`). When accessing the API directly (which Hermes does), the suffix is not needed — just use `gpt-oss:20b`.

---

## Step 3: Create an API Key

Hermes talks to Ollama's cloud via the API, which requires an API key.

1. Go to [ollama.com](https://ollama.com) and sign in
2. Click your **Profile** (top-right) → **Settings** → **API Keys**
3. Click **Create API Key**
4. Copy the key — you'll need it in Step 5

**Set the API key as an environment variable:**
```bash
export OLLAMA_API_KEY=your_api_key_here
```

Add this to your `~/.bashrc`, `~/.zshrc`, or equivalent so it persists across terminal sessions:
```bash
echo 'export OLLAMA_API_KEY=your_api_key_here' >> ~/.bashrc
source ~/.bashrc
```

---

## Step 4: Install Hermes Agent

If you haven't already installed Hermes:

```bash
curl -fsSL https://hermes-agent.nousresearch.com/install.sh | bash
```

**Verify:**
```bash
hermes --version
```

---

## Step 5: Configure Hermes to Use Ollama Cloud

Hermes needs to know where Ollama's cloud API is and which model to use. The easiest way is the interactive setup wizard:

```bash
hermes setup
```

Select the **model** section and choose **Custom endpoint / Ollama**. The wizard will ask for:

| Setting | Value |
|---------|-------|
| Base URL | `https://ollama.com` (Ollama cloud API) |
| API key | *(paste your OLLAMA_API_KEY from Step 3)* |
| Model | `gpt-oss:20b` |

**Alternative — configure manually:**

```bash
hermes config edit
```

Add a custom provider for Ollama cloud and set it as default:

```yaml
model:
  default: gpt-oss:20b
  provider: ollama-cloud

custom_providers:
  - name: ollama-cloud
    base_url: https://ollama.com
    api_key: your_api_key_here
    api_mode: chat_completions
```

Or set it from the command line:

```bash
hermes config set model.provider ollama-cloud
hermes config set model.default gpt-oss:20b
```

> **Model options:** `gpt-oss:20b` is recommended for these exercises — it's a "level 1" (light usage) model that won't consume much of your free tier quota. `minimax-m2.5` is another good option. See [ollama.com/models](https://ollama.com/models) for the full list of cloud-enabled models.

---

## Step 6: Verify Everything Works

Run Hermes's diagnostic check:

```bash
hermes doctor
```

This confirms your config is valid and the Ollama cloud endpoint is reachable.

Then start an interactive session:

```bash
hermes
```

Ask it something:

```
> What is a stack buffer overflow? Give me a one-paragraph explanation.
```

If you get a response — **you're done.** You now have a free, cloud-powered, agentic AI running on your machine with zero GPU requirements.

---

## Troubleshooting

| Problem | Fix |
|---------|-----|
| `ollama: command not found` | Ollama isn't installed or not in your PATH. Re-run the install script. |
| `signin` fails or hangs | Ensure you have a browser available for OAuth. Alternatively, create an account at [ollama.com](https://ollama.com) first, then run `ollama signin`. |
| `401 Unauthorized` from API | Your API key is missing or invalid. Re-check `OLLAMA_API_KEY` is set: `echo $OLLAMA_API_KEY`. Regenerate the key at ollama.com if needed. |
| `429 Too Many Requests` | You've hit your free tier usage limit. Limits reset every 5 hours (session) and every 7 days (weekly). Check your usage via **Profile → Settings → Usage** on [ollama.com](https://ollama.com). |
| Hermes starts but errors on first query | Run `hermes doctor` to diagnose. Check that `model.provider` is set to your Ollama cloud provider name and `model.default` matches a valid cloud model. |
| `model not found` | The model name may have changed. Cloud models are occasionally retired — see [ollama.com/docs](https://docs.ollama.com) for current models. `gpt-oss:20b` and `minimax-m2.5` are stable. |
| Cloud model response is slow | Free tier has standard priority. Responses are still fast (low time-to-first-token) but may queue during peak hours. |

---

## What You Now Have

- **Ollama client** installed and authenticated with ollama.com
- **Ollama cloud API key** configured for programmatic access
- **Hermes Agent** configured to use Ollama cloud as its model provider
- **`gpt-oss:20b`** (and/or **`minimax-m2.5`**) accessible via cloud — no local GPU required

No local GPU. No model downloads. No paid accounts. You can now proceed to Exercise 1.

