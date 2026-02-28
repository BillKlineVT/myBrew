{{/*
Expand the name of the chart.
*/}}
{{- define "mybrew.name" -}}
{{- default .Chart.Name .Values.nameOverride | trunc 63 | trimSuffix "-" }}
{{- end }}

{{/*
Create a default fully qualified app name.
*/}}
{{- define "mybrew.fullname" -}}
{{- if .Values.fullnameOverride }}
{{- .Values.fullnameOverride | trunc 63 | trimSuffix "-" }}
{{- else }}
{{- $name := default .Chart.Name .Values.nameOverride }}
{{- if contains $name .Release.Name }}
{{- .Release.Name | trunc 63 | trimSuffix "-" }}
{{- else }}
{{- printf "%s-%s" .Release.Name $name | trunc 63 | trimSuffix "-" }}
{{- end }}
{{- end }}
{{- end }}

{{/*
Create chart label.
*/}}
{{- define "mybrew.chart" -}}
{{- printf "%s-%s" .Chart.Name .Chart.Version | replace "+" "_" | trunc 63 | trimSuffix "-" }}
{{- end }}

{{/*
Common labels applied to every resource.
*/}}
{{- define "mybrew.labels" -}}
helm.sh/chart: {{ include "mybrew.chart" . }}
app.kubernetes.io/managed-by: {{ .Release.Service }}
app.kubernetes.io/instance: {{ .Release.Name }}
app.kubernetes.io/version: {{ .Chart.AppVersion | quote }}
{{- with .Values.commonLabels }}
{{ toYaml . }}
{{- end }}
{{- end }}

{{/*
Selector labels — used in matchLabels + Service selectors.
Requires a "component" key in the calling context.
Usage:  {{ include "mybrew.selectorLabels" (dict "Release" .Release "component" "api-gateway") }}
*/}}
{{- define "mybrew.selectorLabels" -}}
app.kubernetes.io/name: {{ include "mybrew.name" (dict "Values" .Values "Chart" .Chart "Release" .Release) }}
app.kubernetes.io/instance: {{ .Release.Name }}
app.kubernetes.io/component: {{ .component }}
{{- end }}

{{/*
Return the image tag: prefer per-service tag, fall back to global tag.
Usage: {{ include "mybrew.imageTag" (dict "serviceTag" .Values.apiGateway.image.tag "globalTag" .Values.image.tag) }}
*/}}
{{- define "mybrew.imageTag" -}}
{{- if .serviceTag }}{{ .serviceTag }}{{- else }}{{ .globalTag }}{{- end }}
{{- end }}

{{/*
Service account name.
*/}}
{{- define "mybrew.serviceAccountName" -}}
{{- if .Values.serviceAccount.create }}
{{- default (include "mybrew.fullname" .) .Values.serviceAccount.name }}
{{- else }}
{{- default "default" .Values.serviceAccount.name }}
{{- end }}
{{- end }}

{{/*
PostgreSQL host — supports both embedded subchart and external.
*/}}
{{- define "mybrew.postgresHost" -}}
{{- if .Values.postgresql.enabled }}
{{- printf "%s-postgresql" .Release.Name }}
{{- else }}
{{- .Values.externalPostgresql.host }}
{{- end }}
{{- end }}

{{/*
Redis host — supports both embedded subchart and external.
*/}}
{{- define "mybrew.redisHost" -}}
{{- if .Values.redis.enabled }}
{{- printf "%s-redis-master" .Release.Name }}
{{- else }}
{{- .Values.externalRedis.host }}
{{- end }}
{{- end }}

{{/*
DATABASE_URL for data-service.
*/}}
{{- define "mybrew.databaseUrl" -}}
{{- printf "postgresql+asyncpg://%s:%s@%s:5432/%s" .Values.postgresql.auth.username .Values.postgresql.auth.password (include "mybrew.postgresHost" .) .Values.postgresql.auth.database }}
{{- end }}

{{/*
REDIS_URL for services that connect to Redis.
*/}}
{{- define "mybrew.redisUrl" -}}
{{- printf "redis://%s:6379" (include "mybrew.redisHost" .) }}
{{- end }}
