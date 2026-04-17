// --- Document Ready Function ---
document.addEventListener('DOMContentLoaded', function() {
    // Adapt header navigation for touch/mobile with an accessible toggle
    setupResponsiveNavigation();

    // Handle model loading
    setupModelLoader();

    // Keep active navigation state in sync automatically across pages
    setActiveNavLink();

    // Add contextual gallery captions based on image metadata
    setupGalleryCaptions();

    // Group logs visually by year markers for faster scanning
    insertLogYearMarkers();

    // Enable search/year filtering behavior on logs page
    setupLogsFiltering();

    // Hide/show logs search panel with a compact sticky toggle
    setupLogsPanelToggle();

    // Make log cards easier to scan with optional collapse behavior
    setupLogEntryToggles();

    // Apply image performance defaults for gallery assets
    setupGalleryImagePerformance();
    
    // Add animation for page elements and initialize visible elements
    animateOnScroll();
    initializeVisibleElements();
});

// --- Back to Top Button Logic --- 
let backToTopButton = document.getElementById("back-to-top-btn");

// Improved scroll function with throttling
let scrollTimeout;
window.onscroll = function() {
    if (!scrollTimeout) {
        scrollTimeout = setTimeout(function() {
            scrollFunction();
            scrollTimeout = null;
        }, 100); // Throttle to improve performance
    }
};

function scrollFunction() {
    if (document.body.scrollTop > 200 || document.documentElement.scrollTop > 200) {
        backToTopButton?.classList.add("show");
    } else {
        backToTopButton?.classList.remove("show");
    }
    
    // Add animation to elements when they scroll into view
    const animElements = document.querySelectorAll('.animate-on-scroll:not(.animated)');
    animElements.forEach(element => {
        if (isElementInViewport(element)) {
            element.classList.add('animated');
        }
    });
}

// Check if an element is in viewport
function isElementInViewport(el) {
    const rect = el.getBoundingClientRect();
    return (
        rect.top <= (window.innerHeight || document.documentElement.clientHeight) &&
        rect.bottom >= 0
    );
}

// --- Model Loader Setup ---
function setupModelLoader() {
    const modelViewer = document.querySelector('model-viewer');
    const loadingContainer = document.querySelector('.loading-container');
    const modelHint = document.querySelector('.model-hint');
    
    if (modelViewer && loadingContainer) {
        modelViewer.style.cursor = 'grab';
        modelViewer.addEventListener('mousedown', function() {
            modelViewer.style.cursor = 'grabbing';
        });
        modelViewer.addEventListener('mouseup', function() {
            modelViewer.style.cursor = 'grab';
        });
        modelViewer.addEventListener('mouseleave', function() {
            modelViewer.style.cursor = 'grab';
        });

        modelViewer.addEventListener('load', function() {
            // Hide loading animation when model is loaded
            loadingContainer.style.opacity = '0';
            setTimeout(() => {
                loadingContainer.style.display = 'none';
            }, 500);

            if (modelHint) {
                modelHint.style.opacity = '1';
            }
        });
        
        // Fallback if load event doesn't fire
        setTimeout(() => {
            if (loadingContainer.style.display !== 'none') {
                loadingContainer.style.opacity = '0';
                setTimeout(() => {
                    loadingContainer.style.display = 'none';
                }, 500);
            }
            if (modelHint) {
                modelHint.style.opacity = '1';
            }
        }, 5000);
    }
}

function setActiveNavLink() {
    const navLinks = document.querySelectorAll('nav a');
    if (!navLinks.length) {
        return;
    }

    const currentPage = window.location.pathname.split('/').pop() || 'index.html';
    navLinks.forEach(link => {
        const href = link.getAttribute('href') || '';
        const targetPage = href.split('/').pop();
        if (targetPage && targetPage === currentPage) {
            link.classList.add('active');
        }
    });
}

function setupResponsiveNavigation() {
    const header = document.querySelector('header');
    const nav = header?.querySelector('nav');
    const navList = nav?.querySelector('ul');
    if (!header || !nav || !navList) {
        return;
    }

    if (header.querySelector('.mobile-nav-toggle')) {
        return;
    }

    const toggle = document.createElement('button');
    toggle.type = 'button';
    toggle.className = 'mobile-nav-toggle';
    toggle.setAttribute('aria-label', 'Toggle navigation menu');
    toggle.setAttribute('aria-expanded', 'false');
    toggle.setAttribute('aria-controls', 'primary-nav-list');
    toggle.innerHTML = '<span aria-hidden="true"></span>';

    if (!navList.id) {
        navList.id = 'primary-nav-list';
    }

    const closeMenu = () => {
        document.body.classList.remove('nav-open');
        toggle.setAttribute('aria-expanded', 'false');
    };

    toggle.addEventListener('click', () => {
        const willOpen = !document.body.classList.contains('nav-open');
        document.body.classList.toggle('nav-open', willOpen);
        toggle.setAttribute('aria-expanded', willOpen ? 'true' : 'false');
    });

    nav.addEventListener('click', event => {
        const clickedLink = event.target.closest('a');
        if (clickedLink && window.matchMedia('(max-width: 768px)').matches) {
            closeMenu();
        }
    });

    document.addEventListener('keydown', event => {
        if (event.key === 'Escape') {
            closeMenu();
        }
    });

    window.addEventListener('resize', () => {
        if (window.innerWidth > 768) {
            closeMenu();
        }
    });

    header.insertBefore(toggle, nav);
}

function setupGalleryCaptions() {
    const galleryItems = document.querySelectorAll('.gallery-container .gallery-item');
    if (!galleryItems.length) {
        return;
    }

    galleryItems.forEach(item => {
        const image = item.querySelector('img');
        if (!image || item.querySelector('.gallery-caption')) {
            return;
        }

        const caption = document.createElement('p');
        caption.className = 'gallery-caption';
        caption.textContent = getGalleryCaptionText(image);
        item.appendChild(caption);
    });
}

function getGalleryCaptionText(image) {
    const altText = (image.alt || '').trim();
    const hasGenericAlt = /^gallery image\b/i.test(altText);

    if (altText && !hasGenericAlt) {
        return altText.replace(/\.\d+$/, '').trim();
    }

    const sourcePath = image.getAttribute('src') || '';
    const filename = sourcePath.split('/').pop() || '';
    const baseName = filename.replace(/\.[^.]+$/, '');

    if (!baseName) {
        return 'Project gallery image';
    }

    const formatted = baseName
        .replace(/[-_]+/g, ' ')
        .replace(/\b(vtol|diy|pcb|cad|bms|imu|edf)\b/gi, token => token.toUpperCase())
        .replace(/\b\w/g, ch => ch.toUpperCase())
        .trim();

    return formatted || 'Project gallery image';
}

function insertLogYearMarkers() {
    const logsContainer = document.querySelector('.logs-container');
    if (!logsContainer) {
        return;
    }

    const logEntries = Array.from(logsContainer.querySelectorAll('.log-entry'));
    let lastYear = null;

    logEntries.forEach(entry => {
        const year = entry.getAttribute('data-year');
        if (!year || year === lastYear) {
            return;
        }

        const marker = document.createElement('div');
        marker.className = 'log-year-marker';
        marker.setAttribute('aria-label', `Logs for year ${year}`);
        marker.setAttribute('data-year', year);
        marker.textContent = year;
        logsContainer.insertBefore(marker, entry);
        lastYear = year;
    });
}

function setupLogsFiltering() {
    const logsContainer = document.querySelector('.logs-container');
    const searchInput = document.getElementById('log-search');
    const filterButtons = document.querySelectorAll('.filter-btn');
    const logEntries = Array.from(document.querySelectorAll('.log-entry'));

    if (!logsContainer || !searchInput || !filterButtons.length || !logEntries.length) {
        return;
    }

    let activeYear = 'all';

    const emptyState = document.createElement('p');
    emptyState.className = 'logs-empty-state';
    emptyState.textContent = 'No logs match the current filter.';
    emptyState.hidden = true;
    logsContainer.appendChild(emptyState);

    function applyFilters() {
        const query = searchInput.value.trim().toLowerCase();
        let visibleCount = 0;

        logEntries.forEach(entry => {
            const year = entry.getAttribute('data-year') || '';
            const text = entry.textContent.toLowerCase();
            const matchesYear = activeYear === 'all' || year === activeYear;
            const matchesText = !query || text.includes(query);
            const isVisible = matchesYear && matchesText;

            entry.classList.toggle('is-hidden', !isVisible);
            if (isVisible) {
                visibleCount += 1;
            }
        });

        const yearMarkers = Array.from(logsContainer.querySelectorAll('.log-year-marker'));
        yearMarkers.forEach(marker => {
            const markerYear = marker.getAttribute('data-year');
            const hasVisibleEntriesForYear = logEntries.some(entry => {
                return entry.getAttribute('data-year') === markerYear && !entry.classList.contains('is-hidden');
            });
            marker.classList.toggle('is-hidden', !hasVisibleEntriesForYear);
        });

        emptyState.hidden = visibleCount !== 0;
    }

    filterButtons.forEach(button => {
        button.addEventListener('click', function() {
            filterButtons.forEach(btn => btn.classList.remove('active'));
            this.classList.add('active');

            const id = this.id || '';
            activeYear = id === 'filter-all' ? 'all' : id.replace('filter-', '');
            applyFilters();
        });
    });

    searchInput.addEventListener('input', applyFilters);
    applyFilters();
}

function setupLogEntryToggles() {
    const logHeaders = document.querySelectorAll('.log-entry .log-header');
    if (!logHeaders.length) {
        return;
    }

    logHeaders.forEach(header => {
        const entry = header.closest('.log-entry');
        const content = entry?.querySelector('.log-content');
        if (!entry || !content) {
            return;
        }

        header.setAttribute('role', 'button');
        header.setAttribute('tabindex', '0');
        header.setAttribute('aria-expanded', 'true');

        const toggleEntry = () => {
            const willCollapse = !entry.classList.contains('collapsed');
            entry.classList.toggle('collapsed', willCollapse);
            header.setAttribute('aria-expanded', willCollapse ? 'false' : 'true');
        };

        header.addEventListener('keydown', e => {
            if (e.key === 'Enter' || e.key === ' ') {
                e.preventDefault();
                toggleEntry();
            }
        });

        header.addEventListener('click', () => {
            toggleEntry();
        });
    });
}

function setupLogsPanelToggle() {
    const panel = document.querySelector('.log-filter');
    const mainContent = document.getElementById('main-content');
    if (!panel || !mainContent || document.getElementById('logs-panel-toggle')) {
        return;
    }

    panel.id = 'logs-filter-panel';

    const toggleButton = document.createElement('button');
    toggleButton.id = 'logs-panel-toggle';
    toggleButton.type = 'button';
    toggleButton.className = 'logs-panel-toggle';
    toggleButton.setAttribute('aria-controls', 'logs-filter-panel');

    const savedState = localStorage.getItem('logsPanelOpen');
    const isOpen = savedState === 'true';
    panel.classList.toggle('is-collapsed', !isOpen);
    toggleButton.setAttribute('aria-expanded', isOpen ? 'true' : 'false');
    toggleButton.textContent = isOpen ? 'Hide Search & Filter' : 'Show Search & Filter';

    toggleButton.addEventListener('click', function() {
        const willOpen = panel.classList.contains('is-collapsed');
        panel.classList.toggle('is-collapsed', !willOpen);
        toggleButton.setAttribute('aria-expanded', willOpen ? 'true' : 'false');
        toggleButton.textContent = willOpen ? 'Hide Search & Filter' : 'Show Search & Filter';
        localStorage.setItem('logsPanelOpen', willOpen ? 'true' : 'false');
    });

    mainContent.insertBefore(toggleButton, panel);
}

function setupGalleryImagePerformance() {
    const galleryImages = document.querySelectorAll('.gallery-item img');
    if (!galleryImages.length) {
        return;
    }

    galleryImages.forEach((img, index) => {
        img.loading = index < 12 ? 'eager' : 'lazy';
        img.decoding = 'async';
        img.fetchPriority = index < 12 ? 'high' : 'low';

        if (!img.hasAttribute('width')) {
            img.setAttribute('width', '1200');
        }
        if (!img.hasAttribute('height')) {
            img.setAttribute('height', '900');
        }

        const parentItem = img.closest('.gallery-item');
        if (parentItem) {
            if (img.complete) {
                parentItem.classList.add('image-loaded');
            } else {
                img.addEventListener('load', function() {
                    parentItem.classList.add('image-loaded');
                }, { once: true });
            }
        }
    });
}

// --- Animate on Scroll ---
function animateOnScroll() {
    const elements = document.querySelectorAll('.team-card, .log-entry');
    elements.forEach((el, index) => {
        el.classList.add('animate-on-scroll');
        el.style.transitionDelay = `${Math.min(index, 6) * 0.06}s`;
    });
}

// Initialize elements that are visible on page load
function initializeVisibleElements() {
    // Make initially visible gallery items appear immediately
    const animElements = document.querySelectorAll('.animate-on-scroll');
    animElements.forEach(element => {
        if (isElementInViewport(element)) {
            element.classList.add('animated');
        }
    });
}

// --- Skip Link Focus Fix ---
window.addEventListener('hashchange', function() {
    if (location.hash === '#main-content') {
        const contentMain = document.getElementById('main-content');
        if (contentMain) {
            contentMain.setAttribute('tabindex', '-1');
            contentMain.focus();
        }
    }
});