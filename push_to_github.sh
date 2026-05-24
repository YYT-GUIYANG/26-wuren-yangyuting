#!/bin/bash
#推送到 https://github.com/YYT-GUIYANG/26-wuren-yangyuting.git
set -e
cd "$(dirname "$0")"
REMOTE_HTTPS="https://github.com/YYT-GUIYANG/26-wuren-yangyuting.git"
REMOTE_SSH="git@github.com:YYT-GUIYANG/26-wuren-yangyuting.git"
echo "=== 上传作业2到GitHub ==="
if [ ! -d .git ]; then
    git init -b main
fi
git remote remove origin 2>/dev/null || true
git remote add origin "$REMOTE_HTTPS"
git remote set-url --push origin "$REMOTE_SSH"
#添加作业2所有源码（排除gitignore里的编译产物）
git add -A
git status
if git diff --cached --quiet; then
    echo "没有新改动"
else
    git commit -m "$(cat <<'EOF'
作业2：多元线性回归与Ridge回归

C++手搓LinearRegression和RidgeRegression，加州房价，含notebook。
EOF
)"
fi
git pull origin main --rebase || true
git push -u origin main
echo "完成: $REMOTE_HTTPS"
