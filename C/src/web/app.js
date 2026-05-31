// web/app.js  (Android-patched version)
//
// Primary change from the original:
//   POST requests call window.ConfigBridge.post() directly instead of fetch().
//   This is necessary because Android's WebResourceRequest.shouldInterceptRequest()
//   does not provide the request body for POST calls.
//   GET requests continue to use fetch() as before.

const API_BASE = '/api';

// Detect whether we're running inside the Android WebView
// (ConfigBridge is injected by MainActivity.addJavascriptInterface)
const isAndroid = typeof window.ConfigBridge !== 'undefined';

async function apiCall(endpoint, method, data = null) {
    if (isAndroid && method === 'POST' && data !== null) {
        // ── Android POST path ──────────────────────────────────────────────
        // Calls ConfigBridge.post(path, bodyJson) → nativeHandleRequest via JNI
        try {
            const bodyJson = JSON.stringify(data);
            const resultJson = window.ConfigBridge.post(endpoint, bodyJson);
            return JSON.parse(resultJson);
        } catch (error) {
            console.error('ConfigBridge.post failed:', error);
            return { status: 'ERROR', message: error.message };
        }
    }

    // ── Standard fetch path (GET on Android, everything on Linux/desktop) ──
    const url = isAndroid
        ? `https://appassets.androidplatform.net${API_BASE}${endpoint}`
        : `app://${API_BASE}${endpoint}`;

    const options = {
        method: method,
        headers: { 'Content-Type': 'application/json' }
    };

    if (data) {
        options.body = JSON.stringify(data);
    }

    try {
        const response = await fetch(url, options);
        return await response.json();
    } catch (error) {
        console.error('API call failed:', error);
        return { status: 'ERROR', message: error.message };
    }
}

async function loadConfig() {
    try {
        const result = isAndroid
            ? JSON.parse(window.ConfigBridge.get('/api/config/read'))
            : await apiCall('/config/read', 'GET');

        if (result.status === 'OK') {
            const configDisplay = document.getElementById('configDisplay');
            const text = result.text || '';

            if (text && text.trim()) {
                configDisplay.innerHTML = escapeHtml(text).replace(/\n/g, '<br>');
                updateStatus('Loaded successfully', false);
            } else {
                configDisplay.innerHTML = '<em>No configuration found. Click Edit to create new content.</em>';
                updateStatus('No configuration found', true);
            }
        } else {
            throw new Error('Failed to load config');
        }
    } catch (error) {
        console.error('Error loading config:', error);
        document.getElementById('configDisplay').innerHTML = '<em class="error">Error loading configuration</em>';
        updateStatus('Error loading configuration', true);
    }
}

async function saveConfig(text) {
    const result = await apiCall('/config/write', 'POST', { text: text });

    if (result.status === 'OK') {
        updateStatus('Saved successfully!', false);
        return true;
    } else {
        updateStatus('Error saving configuration!', true);
        return false;
    }
}

function showEditDialog() {
    const currentConfig = document.getElementById('configDisplay').innerText;

    const modal = document.createElement('div');
    modal.className = 'modal';
    modal.id = 'editModal';
    modal.innerHTML = `
        <div class='modal-content'>
            <div class='modal-header'>
                <span class='close'>&times;</span>
                <h2>Edit Configuration</h2>
                <p>Modify the text below and click Save when done.</p>
            </div>
            <div class='modal-body'>
                <textarea id='configTextarea' placeholder='Enter your configuration text here...'></textarea>
            </div>
            <div class='modal-footer'>
                <button class='btn btn-primary' id='modalSaveBtn'>Save</button>
                <button class='btn btn-secondary' id='modalCancelBtn'>Cancel</button>
            </div>
        </div>
    `;

    document.body.appendChild(modal);
    modal.style.display = 'block';

    const textarea = document.getElementById('configTextarea');
    textarea.value = currentConfig;

    const saveBtn    = document.getElementById('modalSaveBtn');
    const cancelBtn  = document.getElementById('modalCancelBtn');
    const closeBtn   = modal.querySelector('.close');

    const closeModal = () => { modal.remove(); };

    saveBtn.onclick = async () => {
        const newText = textarea.value;
        const success = await saveConfig(newText);
        if (success) {
            closeModal();
            await loadConfig();
        }
    };

    cancelBtn.onclick = closeModal;
    closeBtn.onclick  = closeModal;
    modal.onclick = (e) => { if (e.target === modal) closeModal(); };
    textarea.focus();
}

function updateStatus(message, isError = false) {
    const statusLabel = document.getElementById('statusLabel');
    statusLabel.textContent = message;
    statusLabel.className = 'status ' + (isError ? 'error' : 'success');

    setTimeout(() => {
        if (statusLabel.textContent === message) {
            statusLabel.className = 'status';
            statusLabel.textContent = 'Ready';
        }
    }, 3000);
}

function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// Event listeners
document.getElementById('editBtn').addEventListener('click', showEditDialog);
document.getElementById('refreshBtn').addEventListener('click', loadConfig);

// Initial load
loadConfig();
